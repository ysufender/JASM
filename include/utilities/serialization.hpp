#pragma once

#include <cmath>
#include <fstream>
#include <functional>
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
    // TODO:
    //      Add compile-time check for push_back function on Container (de)serialization
    //
    
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