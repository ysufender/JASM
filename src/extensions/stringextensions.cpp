#include <cctype>
#include <cstdint>
#include <sstream>
#include <string>

#include "extensions/stringextensions.hpp"

namespace Extensions::String
{
    std::vector<std::string> Split(const std::string& string, char delimiter, bool removeTrailing)
    {
        std::stringstream ss;
        std::vector<std::string> vec;

        if (string.find_first_of(delimiter) == std::string::npos)
            return { string };
  
        for (size_t i = 0; i < string.size(); i++)
        {
            const char& character { string.at(i) };

            if (character == delimiter)
            {
                vec.push_back(ss.str());
                ss.str(std::string{});
                continue;
            }

            if (removeTrailing && character == ' ' && i != string.size()-1 && string.at(i+1) == ' ')
                continue;

            ss << character;
        }
        
        vec.push_back(ss.str());
        return std::move(vec);
    }

    std::string Concat(const std::vector<std::string>& strings)
    {
        if (strings.size() == 1)
            return strings[0];
        if (strings.size() == 0)
            return "";

        std::stringstream ss;
        for (const auto& str : strings)
            ss << str;
        return std::move(ss.str());
    }

    std::string Join(const std::vector<std::string>& strings, char delimiter)
    {
        if (strings.size() == 1)
            return strings[0];
        if (strings.size() == 0)
            return "";

        std::stringstream ss;
        for (const auto& str : strings)
        {
            ss << str;
            if (str != strings.back())
                ss << delimiter;
        }

        return std::move(ss.str());
    }
}

//
// Tests
//
#include "JASMConfig.hpp"

#ifdef TEST_MODE
#include "test/test.hpp"

TEST_CASE("String Extension Tests")
{
    using namespace Extensions::String;

    SECTION("StringExtensions::Split")
    {
        std::string testStr { "String;; Will be split by; colons" };
        std::vector<std::string> desiredOut { "String", "", " Will be split by", " colons" };

        CHECK(Split(testStr, ';') == desiredOut);
    }

    SECTION("StringExtensions::Concat")
    {
        std::vector<std::string> input { "Nice", " and", " concatenated." };
        std::string desiredOut { "Nice and concatenated." };

        CHECK(Concat(input) == desiredOut);
    }

    SECTION("StringExtensions::Join")
    {
        std::vector<std::string> input { "This","is","without","spaces" };
        std::string desiredOut { "This is without spaces" };

        CHECK(Join(input, ' ') == desiredOut);
    }

    SECTION("StringExtensions::HexToInt")
    {
        std::string hex { "0x0FF05" };
        std::string signedHex { "0xFFFFFFF1" };

        CHECK(HexToInt<int>(hex) == 0x0FF05);
        CHECK(HexToInt<uint32_t>(signedHex) == 0xFFFFFFF1);
        //CHECK_THROWS(HexToInt<float>(hex));
    }
}
#endif
