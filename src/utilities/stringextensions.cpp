#include <cctype>
#include <sstream>
#include <string>

#include "utilities/stringextensions.hpp"

namespace Extensions::String
{
    std::vector<std::string> Split(const std::string& string, char delimiter)
    {
        std::stringstream ss;
        std::vector<std::string> vec;
  
        for (size_t i = 0; i < string.size(); i++)
        {
            const char& character { string.at(i) };

            if (character == delimiter)
            {
                vec.push_back(ss.str());
                ss.str(std::string{});
                continue;
            }

            if (character == ' ' && i != string.size()-1 && string.at(i) == ' ')
                continue;

            ss << character;
        }
        
        vec.push_back(ss.str());
        return std::move(vec);
    }

    std::string Concat(const std::vector<std::string>& strings)
    {
        std::stringstream ss;

        for (const auto& str : strings)
            ss << str;

        return std::move(ss.str());
    }

    std::string Join(const std::vector<std::string>& strings, char delimiter)
    {
        std::stringstream ss;

        for (const auto& str : strings)
            ss << str << delimiter;

        return std::move(ss.str());
    }
}