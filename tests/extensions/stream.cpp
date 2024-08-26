#include <iostream>
#include <array>

#include "catch2/catch_all.hpp"

#include "catch2/catch_test_macros.hpp"
#include "extensions/streamextensions.hpp"

TEST_CASE("Stream Extensions Tests")
{
    using namespace Extensions::Stream;

    std::stringstream ss;
    ss << " fn main(args, argc) {\
                print('Test Stream');\
            }";

    std::istream in { ss.rdbuf() };

    const std::array<std::string, 7> desiredTokens {
        "fn", 
        "main(args,", 
        "argc)", 
        "{",
        "print('Test", 
        "Stream');",
        "}"
    };

    SECTION("StreamExtensions::ForwardTokenize")
    {
        in.seekg(0, std::ios::beg); 

        for (const auto& token : desiredTokens)
            CHECK(Tokenize(in) == token);

        // Gotta double check
        CHECK(Tokenize(in) == "__JASM__EOF__");
        CHECK(Tokenize(in) == "__JASM__EOF__");
    }
}
