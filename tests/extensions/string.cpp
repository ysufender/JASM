#include "catch2/catch_all.hpp"

#include "extensions/stringextensions.hpp"

TEST_CASE("String Extensions Tests")
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
