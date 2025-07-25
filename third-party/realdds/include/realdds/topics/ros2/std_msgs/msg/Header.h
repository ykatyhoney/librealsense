// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2023-5 Intel Corporation. All Rights Reserved.

/*!
 * @file Header.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _FAST_DDS_GENERATED_STD_MSGS_MSG_HEADER_H_
#define _FAST_DDS_GENERATED_STD_MSGS_MSG_HEADER_H_

#include <realdds/topics/ros2/builtin_interfaces/msg/Time.h>

//#include <fastrtps/utils/fixed_size_string.hpp>

#include <stdint.h>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define eProsima_user_DllExport
#endif  // _WIN32

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(Header_SOURCE)
#define Header_DllAPI __declspec( dllexport )
#else
#define Header_DllAPI __declspec( dllimport )
#endif // Header_SOURCE
#else
#define Header_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define Header_DllAPI
#endif // _WIN32

namespace eprosima {
namespace fastcdr {
class Cdr;
} // namespace fastcdr
} // namespace eprosima


namespace std_msgs {
    namespace msg {
        /*!
         * @brief This class represents the structure Header defined by the user in the IDL file.
         * @ingroup HEADER
         */
        class Header
        {
        public:

            /*!
             * @brief Default constructor.
             */
            eProsima_user_DllExport Header();

            /*!
             * @brief Default destructor.
             */
            eProsima_user_DllExport ~Header();

            /*!
             * @brief Copy constructor.
             * @param x Reference to the object std_msgs::msg::Header that will be copied.
             */
            eProsima_user_DllExport Header(
                    const Header& x);

            /*!
             * @brief Move constructor.
             * @param x Reference to the object std_msgs::msg::Header that will be copied.
             */
            eProsima_user_DllExport Header(
                    Header&& x) noexcept;

            /*!
             * @brief Copy assignment.
             * @param x Reference to the object std_msgs::msg::Header that will be copied.
             */
            eProsima_user_DllExport Header& operator =(
                    const Header& x);

            /*!
             * @brief Move assignment.
             * @param x Reference to the object std_msgs::msg::Header that will be copied.
             */
            eProsima_user_DllExport Header& operator =(
                    Header&& x) noexcept;

            /*!
             * @brief Comparison operator.
             * @param x std_msgs::msg::Header object to compare.
             */
            eProsima_user_DllExport bool operator ==(
                    const Header& x) const;

            /*!
             * @brief Comparison operator.
             * @param x std_msgs::msg::Header object to compare.
             */
            eProsima_user_DllExport bool operator !=(
                    const Header& x) const;

            /*!
             * @brief This function copies the value in member stamp
             * @param _stamp New value to be copied in member stamp
             */
            eProsima_user_DllExport void stamp(
                    const builtin_interfaces::msg::Time& _stamp);

            /*!
             * @brief This function moves the value in member stamp
             * @param _stamp New value to be moved in member stamp
             */
            eProsima_user_DllExport void stamp(
                    builtin_interfaces::msg::Time&& _stamp);

            /*!
             * @brief This function returns a constant reference to member stamp
             * @return Constant reference to member stamp
             */
            eProsima_user_DllExport const builtin_interfaces::msg::Time& stamp() const;

            /*!
             * @brief This function returns a reference to member stamp
             * @return Reference to member stamp
             */
            eProsima_user_DllExport builtin_interfaces::msg::Time& stamp();
            /*!
             * @brief This function copies the value in member frame_id
             * @param _frame_id New value to be copied in member frame_id
             */
            eProsima_user_DllExport void frame_id(
                    const std::string& _frame_id);

            /*!
             * @brief This function moves the value in member frame_id
             * @param _frame_id New value to be moved in member frame_id
             */
            eProsima_user_DllExport void frame_id(
                    std::string&& _frame_id);

            /*!
             * @brief This function returns a constant reference to member frame_id
             * @return Constant reference to member frame_id
             */
            eProsima_user_DllExport const std::string& frame_id() const;

            /*!
             * @brief This function returns a reference to member frame_id
             * @return Reference to member frame_id
             */
            eProsima_user_DllExport std::string& frame_id();

            /*!
             * @brief This function returns the maximum serialized size of an object
             * depending on the buffer alignment.
             * @param current_alignment Buffer alignment.
             * @return Maximum serialized size.
             */
            eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
                    size_t current_alignment = 0);

            /*!
             * @brief This function returns the serialized size of a data depending on the buffer alignment.
             * @param data Data which is calculated its serialized size.
             * @param current_alignment Buffer alignment.
             * @return Serialized size.
             */
            eProsima_user_DllExport static size_t getCdrSerializedSize(
                    const std_msgs::msg::Header& data,
                    size_t current_alignment = 0);


            /*!
             * @brief This function serializes an object using CDR serialization.
             * @param cdr CDR serialization object.
             */
            eProsima_user_DllExport void serialize(
                    eprosima::fastcdr::Cdr& cdr) const;

            /*!
             * @brief This function deserializes an object using CDR serialization.
             * @param cdr CDR serialization object.
             */
            eProsima_user_DllExport void deserialize(
                    eprosima::fastcdr::Cdr& cdr);



            /*!
             * @brief This function returns the maximum serialized size of the Key of an object
             * depending on the buffer alignment.
             * @param current_alignment Buffer alignment.
             * @return Maximum serialized size.
             */
            eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
                    size_t current_alignment = 0);

            /*!
             * @brief This function tells you if the Key has been defined for this type
             */
            eProsima_user_DllExport static bool isKeyDefined();

            /*!
             * @brief This function serializes the key members of an object using CDR serialization.
             * @param cdr CDR serialization object.
             */
            eProsima_user_DllExport void serializeKey(
                    eprosima::fastcdr::Cdr& cdr) const;

        private:

            builtin_interfaces::msg::Time m_stamp;
            std::string m_frame_id;
        };
    } // namespace msg
} // namespace std_msgs

#endif // _FAST_DDS_GENERATED_STD_MSGS_MSG_HEADER_H_