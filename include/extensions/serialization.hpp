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
    void inline SerializeInteger(const T& data, std::ofstream& stream)
    {
        stream.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    void inline SerializeFloat(const float& data, std::ofstream& stream)
    {
        stream.write(reinterpret_cast<const char*>(&data), sizeof(float));
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

        UT temp;
        stream.read(reinterpret_cast<char*>(&temp), sizeof(UT));

        if (std::endian::native == std::endian::little)
            data = temp;
        else
        {
            for (char i = 0; i < sizeof(UT); i++)
            {
                uchar swapData { static_cast<uchar>(temp >> i*8) };
                data <<= 8;
                data |= swapData;
            }
        }
    }

    void DeserializeFloat(float& data, std::ifstream& stream);
    

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