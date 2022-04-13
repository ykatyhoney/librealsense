// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2022 Intel Corporation. All Rights Reserved.

#include <iostream>

#include "dds-server.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/rtps/participant/ParticipantDiscoveryInfo.h>
#include <src/dds/msg/devicesPubSubTypes.h>

// We align the DDS topic name to ROS2 as it expect the 'rt/' prefix for the topic name
#define ROS2_PREFIX( name ) std::string( "rt/" ).append( name )

using namespace eprosima::fastdds::dds;

dds_server::dds_server()
    : _running( false )
    , _participant( nullptr )
    , _publisher( nullptr )
    , _topic( nullptr )
    , _type_support_ptr( new devicesPubSubType() )
    , _dds_device_dispatcher( 10 )
    , _ctx( "{"
            "\"dds-discovery\" : false"
            "}" )
{
}
bool dds_server::init( DomainId_t domain_id )
{  
    auto participant_created_ok = create_dds_participant( domain_id );
    auto publisher_created_ok = create_dds_publisher();
    return participant_created_ok && publisher_created_ok;
}

void dds_server::run()
{
    _dds_device_dispatcher.start();
    _running = true;
    
    post_connected_devices_on_wakeup();

    // Register to LRS device changes function to notify on future devices being connected/disconnected
    _ctx.set_devices_changed_callback( [this]( rs2::event_information & info ) {
        if( _running )
        {
            std::vector< std::string > devices_to_remove;
            std::vector< std::pair< std::string, rs2::device > > devices_to_add;

            if( prepare_devices_changed_lists( info, devices_to_remove, devices_to_add ) )
            {
                // Post the devices connected / removed
                _dds_device_dispatcher.invoke(
                    [this, devices_to_remove, devices_to_add]( dispatcher::cancellable_timer c ) {
                        post_device_changes( devices_to_remove, devices_to_add );
                    } );
            }
        }
    } );

    std::cout << "RS DDS Server is on.." << std::endl;
}

bool dds_server::prepare_devices_changed_lists(
    const rs2::event_information & info,
    std::vector< std::string > &devices_to_remove,
    std::vector< std::pair< std::string, rs2::device > > &devices_to_add )
{
    // Remove disconnected devices from devices list
    for( auto dev_info : _devices_writers )
    {
        auto & dev = dev_info.second.device;
        auto device_name = dev.get_info( RS2_CAMERA_INFO_NAME );

        if( info.was_removed( dev ) )
        {
            devices_to_remove.push_back( device_name );
        }
    }

    // Add new connected devices from devices list
    for( auto && dev : info.get_new_devices() )
    {
        auto device_name = dev.get_info( RS2_CAMERA_INFO_NAME );
        devices_to_add.push_back( { device_name, dev } );
    }

    bool device_change_detected = !devices_to_remove.empty() || !devices_to_add.empty();
    return device_change_detected;
}

void dds_server::post_device_changes(
    std::vector< std::string > devices_to_remove,
    std::vector< std::pair< std::string, rs2::device > > devices_to_add )
{
    try
    {
        for( auto dev_to_remove : devices_to_remove )
        {
            remove_dds_device( dev_to_remove );
        }

        for( auto dev_to_add : devices_to_add )
        {
            add_dds_device( dev_to_add );
        }
    }

    catch( ... )
    {
        std::cout << "Unknown error when trying to remove/add a DDS device" << std::endl;
    }
}

void dds_server::remove_dds_device( std::string device_name )
{
    // deleting a device also notify the clients internally
    _publisher->delete_datawriter( _devices_writers[device_name].data_writer );
    std::cout << "Device '" << device_name << "' - removed" << std::endl;
    _devices_writers.erase( device_name );
}

void dds_server::add_dds_device( std::pair< std::string, rs2::device > device_pair )
{
    auto dev_name = device_pair.first;
    auto rs2_dev = device_pair.second;

    if( !create_device_writer( dev_name, rs2_dev) )
    {
        std::cout << "Error creating a DDS writer" << std::endl;
        return;
    }

    // Publish the device info, but only after a matching reader is found.
    devices msg;
    strcpy( msg.name().data(), dev_name.c_str() );
    std::cout << "\nDevice '" << dev_name << "' - detected" << std::endl;
    std::cout << "Looking for at least 1 matching reader... ";  // Status value will be appended to this line
    
    if( is_client_exist( dev_name ) )
    {
        std::cout << "found" << std::endl;
        // Post a DDS message with the new added device
        if( _devices_writers[dev_name].data_writer->write( &msg ) )
        {
            std::cout << "DDS device message sent!" << std::endl;
        }
        else
        {
            std::cout << "Error writing new device message for: " << dev_name << std::endl;
        }
    }
    else
    {
        std::cout << "not found" << std::endl;
        std::cout << "Timeout finding a reader for devices topic" << std::endl;
    }
}

bool dds_server::is_client_exist( const std::string &device_name ) const
{
    int retry_cnt = 4;
    bool client_found = false;
    do
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 250 ) );
        client_found = _devices_writers.at(device_name).listener->_matched != 0;
    }
    while( ! client_found && retry_cnt-- > 0 );

    return client_found;
}

bool dds_server::create_device_writer( std::string device_name, rs2::device rs2_device )
{
    // Create a data writer for the topic
    DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
    wqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    wqos.durability().kind = VOLATILE_DURABILITY_QOS;
    wqos.data_sharing().automatic();
    wqos.ownership().kind = EXCLUSIVE_OWNERSHIP_QOS;
    std::shared_ptr< dds_serverListener > writer_listener
        = std::make_shared< dds_serverListener >();

    _devices_writers[device_name]
        = { rs2_device,
            _publisher->create_datawriter( _topic, wqos, writer_listener.get() ),
            writer_listener };

    return _devices_writers[device_name].data_writer != nullptr;
}

bool dds_server::create_dds_participant( DomainId_t domain_id )
{
    DomainParticipantQos pqos;
    pqos.name( "rs-dds-server" );
    _participant
        = DomainParticipantFactory::get_instance()->create_participant( domain_id,
                                                                        pqos,
                                                                        &_domain_listener );
    return _participant != nullptr;
}

bool dds_server::create_dds_publisher()
{
    // Registering the topic type enables topic instance creation by factory
    _type_support_ptr.register_type( _participant );
    _publisher = _participant->create_publisher( PUBLISHER_QOS_DEFAULT, nullptr );
    _topic = _participant->create_topic( ROS2_PREFIX( "Devices" ),
                                         _type_support_ptr->getName(),
                                         TOPIC_QOS_DEFAULT );

    return ( _topic != nullptr && _publisher != nullptr );
}

void dds_server::post_connected_devices_on_wakeup()
{
    // Query the devices connected on startup
    auto connected_dev_list = _ctx.query_devices();
    std::vector< std::pair< std::string, rs2::device > > devices_to_add;
    for( auto connected_dev : connected_dev_list )
    {
        auto device_name = connected_dev.get_info( RS2_CAMERA_INFO_NAME );
        devices_to_add.push_back( { device_name, connected_dev } );
    }

    if( ! devices_to_add.empty() )
    {
        // Post the devices connected on startup
        _dds_device_dispatcher.invoke( [this, devices_to_add]( dispatcher::cancellable_timer c ) {
            post_device_changes( {}, devices_to_add );
        } );
    }
}

dds_server::~dds_server()
{
    std::cout << "Shutting down rs-dds-server..." << std::endl;
    _running = false;

    _dds_device_dispatcher.stop();

    for( auto device_writer : _devices_writers )
    {
        auto & dev_writer = device_writer.second.data_writer;
        if( dev_writer )
            _publisher->delete_datawriter( dev_writer );
    }
    _devices_writers.clear();

    if( _topic != nullptr )
    {
        _participant->delete_topic( _topic );
    }
    if( _publisher != nullptr )
    {
        _participant->delete_publisher( _publisher );
    }
    DomainParticipantFactory::get_instance()->delete_participant( _participant );
}


void dds_server::dds_serverListener::on_publication_matched( DataWriter * writer,
                                                             const PublicationMatchedStatus & info )
{
    if( info.current_count_change == 1 )
    {
        std::cout << "DataReader " << writer->guid() << " discovered" << std::endl;
        _matched = info.total_count;
    }
    else if( info.current_count_change == -1 )
    {
        std::cout << "DataReader " << writer->guid() << " disappeared" << std::endl;
        _matched = info.total_count;
    }
    else
    {
        std::cout << std::to_string( info.current_count_change )
                  << " is not a valid value for on_publication_matched" << std::endl;
    }
}

void dds_server::DiscoveryDomainParticipantListener::on_participant_discovery(
    DomainParticipant * participant, eprosima::fastrtps::rtps::ParticipantDiscoveryInfo && info )
{
    switch( info.status )
    {
    case eprosima::fastrtps::rtps::ParticipantDiscoveryInfo::DISCOVERED_PARTICIPANT:
        std::cout << "Participant '" << info.info.m_participantName << "' discovered" << std::endl;
        break;
    case eprosima::fastrtps::rtps::ParticipantDiscoveryInfo::REMOVED_PARTICIPANT:
    case eprosima::fastrtps::rtps::ParticipantDiscoveryInfo::DROPPED_PARTICIPANT:
        std::cout << "Participant '" << info.info.m_participantName << "' disappeared" << std::endl;
        break;
    default:
        break;
    }
}
