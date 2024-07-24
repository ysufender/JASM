#pragma once

#include <cstdint>
#include <fstream>

namespace Extensions::Serialization
{
    //
    // Primitives
    //
    // TODO: 
    // i32, u16, u32, u64
    // byte, ubyte
    // float
    void SerializeInt32(const uint32_t& data, std::ofstream& stream);
    void SerializeInt16(const uint32_t& data, std::ofstream& stream);
    void SerializeUnsigned32(const uint32_t& data, std::ofstream& stream);
    void SerializeUnsigned64(const uint32_t& data, std::ofstream& stream);
    void SerializeByte(const uint32_t& data, std::ofstream& stream);
    void SerializeUbyte(const uint32_t& data, std::ofstream& stream);
    void SerializeFloat(const uint32_t& data, std::ofstream& stream);
}