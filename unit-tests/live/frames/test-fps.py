# License: Apache 2.0. See LICENSE file in root directory.
# Copyright(c) 2022 Intel Corporation. All Rights Reserved.

# test:device each(D400*)
# test:donotrun:!nightly

import pyrealsense2 as rs
from rspy.stopwatch import Stopwatch
from rspy import test, log
import time
import platform

# Start depth + color streams and measure frame frequency using sensor API.
# Verify that actual fps is as requested

global first_frame_seconds

def measure_fps(sensor, profile, seconds_to_count_frames = 10):
    """
    Wait a few seconds to be sure that frames are at steady state after start
    Count number of received frames for seconds_to_count_frames seconds and compare actual fps to requested fps
    """
    seconds_till_steady_state = 4

    steady_state = False
    first_frame_received = False
    frames_received = 0
    first_frame_stopwatch = Stopwatch()
    prev_frame_number = 0

    def frame_cb(frame):
        global first_frame_seconds
        nonlocal steady_state, frames_received, first_frame_received, prev_frame_number
        current_frame_number = frame.get_frame_number()
        if not first_frame_received:
            first_frame_seconds = first_frame_stopwatch.get_elapsed()
            first_frame_received = True
        else:
            if current_frame_number > prev_frame_number + 1:
                log.w( f'Frame drop detected. Current frame number {current_frame_number} previous was {prev_frame_number}' )
        if steady_state:
            frames_received += 1
        prev_frame_number = current_frame_number

    sensor.open(profile)
    sensor.start(frame_cb)
    first_frame_stopwatch.reset()

    time.sleep(seconds_till_steady_state)

    steady_state = True

    time.sleep(seconds_to_count_frames) # Time to count frames

    steady_state = False # Stop counting

    sensor.stop()
    sensor.close()

    fps = frames_received / seconds_to_count_frames
    return fps


delta_Hz = 1
tested_fps = [5, 6, 15, 30, 60, 90]
time_to_test_fps = [25, 20, 13, 10, 5, 4]
test.check_equal( len(tested_fps), len(time_to_test_fps) )

dev, _ = test.find_first_device_or_exit()
product_line = dev.get_info(rs.camera_info.product_line)

#####################################################################################################
test.start("Testing depth fps " + product_line + " device - "+ platform.system() + " OS")

ds = dev.first_depth_sensor()
# Set auto-exposure option as it might take precedence over requested FPS
if product_line == "D400":
    if ds.supports(rs.option.enable_auto_exposure):
        ds.set_option(rs.option.enable_auto_exposure, 1)

for i in range(len(tested_fps)):
    requested_fps = tested_fps[i]    
    try:
        dp = next(p for p in ds.profiles
                  if p.fps() == requested_fps
                  and p.stream_type() == rs.stream.depth
                  and p.format() == rs.format.z16)
    except StopIteration:
        log.i("Requested fps: {:.1f} [Hz], not supported".format(requested_fps))
    else:
        fps = measure_fps(ds, dp, time_to_test_fps[i])
        log.i("Requested fps: {:.1f} [Hz], actual fps: {:.1f} [Hz]. Time to first frame {:.6f}".format(requested_fps, fps, first_frame_seconds))
        delta_Hz = requested_fps * 0.05 # Validation KPI is 5%
        test.check(fps <= (requested_fps + delta_Hz) and fps >= (requested_fps - delta_Hz))
test.finish()


#####################################################################################################
test.start("Testing color fps " + product_line + " device - "+ platform.system() + " OS")

product_name = dev.get_info(rs.camera_info.name)
cs = None
try:
    cs = dev.first_color_sensor()
except RuntimeError as rte:
    if 'D421' not in product_name and 'D405' not in product_name: # Cameras with no color sensor may fail.
        test.unexpected_exception()

if cs:        
    # Set auto-exposure option as it might take precedence over requested FPS
    if product_line == "D400":
        if cs.supports(rs.option.enable_auto_exposure):
            cs.set_option(rs.option.enable_auto_exposure, 1)
        if cs.supports(rs.option.auto_exposure_priority):
            cs.set_option(rs.option.auto_exposure_priority, 0) # AE priority should be 0 for constant FPS

    for i in range(len(tested_fps)):
        requested_fps = tested_fps[i]
        try:
            cp = next(p for p in cs.profiles
                      if p.fps() == requested_fps
                      and p.stream_type() == rs.stream.color
                      and p.format() == rs.format.rgb8)
        except StopIteration:
            log.i("Requested fps: {:.1f} [Hz], not supported".format(requested_fps))
        else:
            fps = measure_fps(cs, cp, time_to_test_fps[i])
            log.i("Requested fps: {:.1f} [Hz], actual fps: {:.1f} [Hz]. Time to first frame {:.6f}".format(requested_fps, fps, first_frame_seconds))
            delta_Hz = requested_fps * (0.10 if requested_fps == 5 else 0.05) # Validation KPI is 5% for all non 5 FPS rate
            test.check(fps <= (requested_fps + delta_Hz) and fps >= (requested_fps - delta_Hz))

test.finish()

#####################################################################################################
test.print_results_and_exit()
