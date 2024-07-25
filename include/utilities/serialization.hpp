#pragma once

#include "utilities/stringextensions.hpp"
#include "utilities/system.hpp"
#include <cmath>
#include <concepts>
#include <fstream>
#include <functional>
#include <string>
#include <type_traits>

namespace Extensions::Serialization
{
    template<typename T>
    concept integer = std::integral<T>;
    
    //
    // Serialization
    //
    template<typename T>
        requires integer<T>
    void SerializeInteger(const T& data, std::ofstream& stream)
    {
        for (char i = 0; i < sizeof(data); i++)
        {
            char byte { static_cast<char>(data >> 8*i) };
            stream.write(&byte, sizeof(byte));
        }
    }

    template<typename ContT, typename SizeT, typename ElemT>
        requires integer<SizeT>
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
    template<typename T>
        requires integer<T>
    void DeserializeInteger(T& data, std::ifstream& stream)
    {
        T temp { 0 };
        for (char i = 0; i < sizeof(T); i++)
        {
            temp <<= 8;
            char byte;
            stream.read(&byte, sizeof(byte));
            temp |= byte;
        }

        for (char i = 0; i < sizeof(T); i++)
        {
            char swapData { static_cast<char>(temp >> i*8) };
            data <<= 8;
            data |= static_cast<std::make_unsigned<char>::type>(swapData);
        }
    }

    template<typename ContT, typename SizeT, typename ElemT>
        requires integer<SizeT>
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