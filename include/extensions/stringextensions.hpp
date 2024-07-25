#pragma once

#include <concepts>
#include <string>
#include <vector>

namespace Extensions::String 
{
    std::vector<std::string> Split(const std::string& str, char delimiter);
    std::string Concat(const std::vector<std::string>& strings);
    std::string Join(const std::vector<std::string>& strings, char delimitier);

    template<typename T>
    concept integer = std::integral<T>;
        
    template<integer T>
    inline constexpr T HexToInt(const std::string& hex)
    {
        return static_cast<T>(std::stoul(hex, 0, 16));
    }
}