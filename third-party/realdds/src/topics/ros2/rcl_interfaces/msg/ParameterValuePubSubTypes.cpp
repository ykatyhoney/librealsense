// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2025 Intel Corporation. All Rights Reserved.

/*!
 * @file ParameterValuePubSubTypes.cpp
 * This header file contains the implementation of the serialization functions.
 *
 * This file was generated by the tool fastcdrgen.
 */


#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>

#include <realdds/topics/ros2/rcl_interfaces/msg/ParameterValuePubSubTypes.h>

using SerializedPayload_t = eprosima::fastrtps::rtps::SerializedPayload_t;
using InstanceHandle_t = eprosima::fastrtps::rtps::InstanceHandle_t;

namespace rcl_interfaces {
    namespace msg {
        ParameterValuePubSubType::ParameterValuePubSubType()
        {
            setName("rcl_interfaces::msg::dds_::ParameterValue_");
            auto type_size = ParameterValue::getMaxCdrSerializedSize();
            type_size += eprosima::fastcdr::Cdr::alignment(type_size, 4); /* possible submessage alignment */
            m_typeSize = static_cast<uint32_t>(type_size) + 4; /*encapsulation*/
            m_isGetKeyDefined = ParameterValue::isKeyDefined();
            size_t keyLength = ParameterValue::getKeyMaxCdrSerializedSize() > 16 ?
                    ParameterValue::getKeyMaxCdrSerializedSize() : 16;
            m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
            memset(m_keyBuffer, 0, keyLength);
        }

        ParameterValuePubSubType::~ParameterValuePubSubType()
        {
            if (m_keyBuffer != nullptr)
            {
                free(m_keyBuffer);
            }
        }

        bool ParameterValuePubSubType::serialize(
                void* data,
                SerializedPayload_t* payload)
        {
            ParameterValue* p_type = static_cast<ParameterValue*>(data);

            // Object that manages the raw buffer.
            eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
            // Object that serializes the data.
            eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
            payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
            // Serialize encapsulation
            ser.serialize_encapsulation();

            try
            {
                // Serialize the object.
                p_type->serialize(ser);
            }
            catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
            {
                return false;
            }

            // Get the serialized length
            payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
            return true;
        }

        bool ParameterValuePubSubType::deserialize(
                SerializedPayload_t* payload,
                void* data)
        {
            try
            {
                //Convert DATA to pointer of your type
                ParameterValue* p_type = static_cast<ParameterValue*>(data);

                // Object that manages the raw buffer.
                eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

                // Object that deserializes the data.
                eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

                // Deserialize encapsulation.
                deser.read_encapsulation();
                payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

                // Deserialize the object.
                p_type->deserialize(deser);
            }
            catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
            {
                return false;
            }

            return true;
        }

        std::function<uint32_t()> ParameterValuePubSubType::getSerializedSizeProvider(
                void* data)
        {
            return [data]() -> uint32_t
                   {
                       return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<ParameterValue*>(data))) +
                              4u /*encapsulation*/;
                   };
        }

        void* ParameterValuePubSubType::createData()
        {
            return reinterpret_cast<void*>(new ParameterValue());
        }

        void ParameterValuePubSubType::deleteData(
                void* data)
        {
            delete(reinterpret_cast<ParameterValue*>(data));
        }

        bool ParameterValuePubSubType::getKey(
                void* data,
                InstanceHandle_t* handle,
                bool force_md5)
        {
            if (!m_isGetKeyDefined)
            {
                return false;
            }

            ParameterValue* p_type = static_cast<ParameterValue*>(data);

            // Object that manages the raw buffer.
            eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
                    ParameterValue::getKeyMaxCdrSerializedSize());

            // Object that serializes the data.
            eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
            p_type->serializeKey(ser);
            if (force_md5 || ParameterValue::getKeyMaxCdrSerializedSize() > 16)
            {
                m_md5.init();
                m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
                m_md5.finalize();
                for (uint8_t i = 0; i < 16; ++i)
                {
                    handle->value[i] = m_md5.digest[i];
                }
            }
            else
            {
                for (uint8_t i = 0; i < 16; ++i)
                {
                    handle->value[i] = m_keyBuffer[i];
                }
            }
            return true;
        }


    } //End of namespace msg

} //End of namespace rcl_interfaces
