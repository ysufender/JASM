#include <cctype>
#include <sstream>
#include <string>

#include "extensions/stringextensions.hpp"

namespace Extensions::String
{
    std::vector<std::string> Split(const std::string_view& str, char delimiter, bool removeTrailing)
    {
        std::stringstream ss;
        std::vector<std::string> vec;

        if (str.find_first_of(delimiter) == std::string::npos)
            return { str.data() };
  
        for (size_t i = 0; i < str.size(); i++)
        {
            const char& character { str.at(i) };

            if (character == delimiter)
            {
                vec.push_back(ss.str());
                ss.str(std::string{});
                continue;
            }

            if (removeTrailing && character == ' ' && i != str.size()-1 && str.at(i+1) == ' ')
                continue;

            ss << character;
        }
        
        vec.push_back(ss.str());
        return vec;
    }

    std::string Concat(const std::vector<std::string_view>& strings)
    {
        if (strings.size() == 1)
            return strings[0].data();
        if (strings.size() == 0)
            return "";

        std::stringstream ss;
        for (const auto& str : strings)
            ss << str;
        return ss.str();
    }

    std::string Join(const std::vector<std::string_view>& strings, char delimiter)
    {
        if (strings.size() == 1)
            return strings[0].data();
        if (strings.size() == 0)
            return "";

        std::stringstream ss;
        for (const auto& str : strings)
        {
            ss << str;
            if (str != strings.back())
                ss << delimiter;
        }

        return ss.str();
    }

    size_t Hash(std::string_view str)
    {
        static constexpr auto hasher { std::hash<std::string_view>{} };;
        return hasher(str);
    }
}
