#pragma once

#include "extensions/system.hpp"
#include <bit>
#include <cmath>
#include <fstream>
#include <functional>
#include <ios>
#include <iterator>
#include <string>
#include <type_traits>

namespace Extensions::Serialization
{
    template<typename T>
    concept integer = std::integral<T>;

    template<typename T>
    concept iterable = requires(T& t)
    {
        std::begin(t) != std::end(t);
        ++std::declval<decltype(std::begin(t))&>();
        *std::begin(t);
    };


    //
    // Serialization
    //
    template<integer T>
    void SerializeInteger(const T& data, std::ofstream& stream)
    {
        for (char i = 0; i < sizeof(data); i++)
        {
            char byte { static_cast<char>(data >> 8*i) };
            stream.write(&byte, sizeof(byte));
        }
    }

    void constexpr SerializeFloat(const float& data, std::ofstream& stream)
    {
        stream.write(reinterpret_cast<const char*>(&data), sizeof(data));
    }

    template<iterable ContT, integer SizeT, typename ElemT>
    void SerializeContainer(
            const ContT& container, 
            std::function<void(const ElemT&, std::ofstream&)> serializer, 
            std::ofstream& stream
    )
    {
        SizeT size { static_cast<SizeT>(container.size()) };
        SerializeInteger(size, stream);

        for (const ElemT& element : container)
           serializer(element, stream); 
    }

    //
    // Deserialization
    //
    template<integer T>
    void DeserializeInteger(T& data, std::ifstream& stream)
    {
        using uchar = std::make_unsigned_t<char>;
        using UT = std::make_unsigned_t<T>;

        UT temp { 0 };
        for (char i = 0; i < sizeof(T); i++)
        {
            temp <<= 8;
            uchar byte;
            stream.read(reinterpret_cast<char*>(&byte), sizeof(byte));
            temp |= byte;
        }

        for (char i = 0; i < sizeof(T); i++)
        {
            uchar swapData { static_cast<uchar>(temp >> i*8) };
            data <<= 8;
            data |= swapData;
        }
    }

    void constexpr DeserializeFlaot(float& data, std::ifstream& stream)
    {
        float flTemp; 
        stream.read(reinterpret_cast<char*>(&flTemp), sizeof(flTemp));

        if (std::endian::native == std::endian::little)
            data = flTemp;
        else
        {

        }
    }

    template<iterable ContT, integer SizeT, typename ElemT>
    void DeserializeContainer(
            ContT& container,
            std::function<void(ElemT&, std::ifstream&)> deserializer,
            std::ifstream& stream
    )
    {
        SizeT size;
        DeserializeInteger(size, stream);
        
        for (; size > 0; size--)
        {
            ElemT element { };
            deserializer(element, stream);
            container.push_back(std::move(element));
        }
    }
}