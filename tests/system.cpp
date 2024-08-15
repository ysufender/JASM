#include "assemblycontext.hpp"
#include "catch2/catch_test_macros.hpp"
#include <filesystem>

#include "system.hpp"

TEST_CASE("System Tests")
{
    using namespace Extensions::String;
    std::stringstream in {};

    SECTION("System::Setup")
    {
        System::Setup(DefaultContext, in, in);

        CHECK(std::cout.rdbuf() == in.rdbuf());
        CHECK(std::cerr.rdbuf() == in.rdbuf());

        LOG("Hello");
        CHECK(in.str() == "Hello\n");
        CHECK(std::filesystem::current_path() == DefaultContext.WorkingDir());
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
            CHECK(in.str() == Concat({"[JASM::Log](JASM/tests/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
        }

        SECTION("System::LogTest::LogWarning")
        {
            LOGW("Testing Testing...");
            CHECK(in.str() == Concat({"[JASM::Warning](JASM/tests/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
        }

        SECTION("System::LogTest::LogError")
        {
            SECTION("System::LogTest::LogError::Normal")
            {
                LOGE(System::LogLevel::Normal, "Testing Testing...");
                CHECK(in.str() == Concat({"[JASM::Error](JASM/tests/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
            }

            SECTION("System::LogTest::LogError::Low")
            {
                LOGE(System::LogLevel::Low, "Testing Testing...");
                CHECK(in.str() == Concat({"ERROR [JASM::Error](JASM/tests/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
            }

            SECTION("System::LogTest::LogError::Medium")
            {
                LOGE(System::LogLevel::Medium, "Testing Testing...");
                CHECK(in.str() == Concat({"IMPORTANT ERROR [JASM::Error](JASM/tests/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
            }

            SECTION("System::LogTest::LogError::High")
            {
                CHECK_THROWS_AS(
                    LOGE(System::LogLevel::High, "Testing Testing..."),
                    JASMException
                );

                CHECK(in.str() == Concat({"ALERT JASM/tests/system.cpp:", std::to_string(__LINE__-4), "\n"}));
            }
        }
    }
}
