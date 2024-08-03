#include "test.hpp"

#include "extensions/system.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "extensions/stringextensions.hpp"

TEST_CASE("System Tests")
{
    using namespace Extensions::String;
    std::stringstream in {};

    SECTION("System::Setup")
    {
        System::Setup(DefaultContext, in, in);

        CHECK(std::cout.rdbuf() == in.rdbuf());
        CHECK(std::cerr.rdbuf() == in.rdbuf());
    }

    SECTION("System::LogTest")
    {
        SECTION("System::LogTest::Log")
        {
            LOG("Testing Testing...");
            CHECK(in.str() == "Testing Testing...\n");
        }

        SECTION("System::LogTest::LogInternal")
        {
            LOGD("Testing Testing...");
            CHECK(in.str() == Concat({"[JASM::Log](JASM/tests/extensions.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
        }

        SECTION("System::LogTest::LogWarning")
        {
            LOGW("Testing Testing...");
            CHECK(in.str() == Concat({"[JASM::Warning](JASM/tests/extensions.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
        }

        SECTION("System::LogTest::LogError")
        {
            SECTION("System::LogTest::LogError::Normal")
            {
                LOGE(System::LogLevel::Normal, "Testing Testing...");
                CHECK(in.str() == Concat({"[JASM::Error](JASM/tests/extensions.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
            }

            SECTION("System::LogTest::LogError::Low")
            {
                LOGE(System::LogLevel::Low, "Testing Testing...");
                CHECK(in.str() == Concat({"ERROR [JASM::Error](JASM/tests/extensions.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
            }

            SECTION("System::LogTest::LogError::Medium")
            {
                LOGE(System::LogLevel::Medium, "Testing Testing...");
                CHECK(in.str() == Concat({"IMPORTANT ERROR [JASM::Error](JASM/tests/extensions.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
            }

            SECTION("System::LogTest::LogError::High")
            {
                CHECK_THROWS_WITH(
                    LOGE(System::LogLevel::High, "Testing Testing..."),
                    "Testing Testing..."
                );

                CHECK(in.str() == Concat({"ALERT JASM/tests/extensions.cpp:", std::to_string(__LINE__-4), "\n"}));
            }
        }
    }
}


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


TEST_CASE("Stream Extensions Tests")
{
    using namespace Extensions::Stream;

    std::stringstream ss;
    ss << " fn main(args, argc) {\
                print('Test Stream');\
            }";

    std::istream in { ss.rdbuf() };

    const std::string desiredTokens[] {
        "fn", "main(args,", "argc)", "{",
        "print('Test", "Stream');",
        "}"
    };

    SECTION("StreamExtensions::ForwardTokenize")
    {
        in.seekg(0, std::ios::beg); 

        for (const auto& token : desiredTokens)
            CHECK(Tokenize(in) == token);

        // Gotta double check
        CHECK(Tokenize(in) == "EOF");
        CHECK(Tokenize(in) == "EOF");
    }

    SECTION("StreamExtensions::BackwardTokenize")
    {
        in.seekg(0, std::ios::end); 

        for (int i = 6; i >= 0; i--)
            CHECK(Tokenize(in, true) == desiredTokens[i]);

        // Gotta double check
        CHECK(Tokenize(in, true) == "SOF");
        CHECK(Tokenize(in, true) == "SOF");
    }
}


TEMPLATE_TEST_CASE( "(De)Serialization Tests", "", 
    int, char, short, long, long long, 
    uint32_t, uint64_t, uint16_t, uint8_t
)
{
}
