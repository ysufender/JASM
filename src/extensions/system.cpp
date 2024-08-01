#include <iostream>
#include <stdexcept>
#include <ostream>
#include <string>
#include <string_view>

#include "extensions/system.hpp"
#include "assemblycontext.hpp"
#include "extensions/stringextensions.hpp"

using systembit_t = uint32_t;

AssemblyContext& System::Context { DefaultContext };

void System::Setup(const AssemblyContext& context, std::ostream& cout, std::ostream& cerr)
{
    Context = context;
    std::cout.rdbuf(cout.rdbuf());
    std::cerr.rdbuf(cerr.rdbuf());
}

void System::Log(std::string_view message)
{
    if (Context.IsSilent())
        return;

    std::cout << message << '\n';
}

void System::LogInternal(std::string_view message, std::string_view file, int line)
{
    if (Context.IsSilent())
        return;

    size_t idx { file.find_first_of("JASM") };
    std::cout << "[JASM::Log](" << file.substr(idx, file.size() - idx) << ":" << line << ") >>> "  << message << '\n';
}

void System::LogWarning(std::string_view message, std::string_view file, int line)
{
    if (Context.IsSilent())
        return;

    size_t idx { file.find_first_of("JASM") };
    std::cout << "[JASM::Warning](" << file.substr(idx, file.size() - idx) << ':' << line << ") >>> " << message << '\n';
}

void System::LogError(std::string_view message, LogLevel level, std::string_view file, int line)
{
    size_t idx { file.find_first_of("JASM") };

    switch (level)
    {
        case System::LogLevel::Normal:
            {
                if (Context.IsSilent())
                    return;
            }
            break;
        case System::LogLevel::Low:
            {
                if (Context.IsSilent())
                    return;

                std::cout << "ERROR ";
            }
            break;
        case System::LogLevel::Medium:
            std::cout << "IMPORTANT ERROR ";
            break;
        case System::LogLevel::High:
            std::cout  << "ALERT " << file.substr(idx, file.size() - idx) << ':' << line << '\n';
            throw std::runtime_error(message.data());
            break;
    }

    std::cerr << "[JASM::Error](" << file.substr(idx, file.size() - idx) << ':' << line << ") >>> " << message << '\n';
}

//
// Tests
//
//
#include "JASMConfig.hpp"
#include "test/test.hpp"

#ifdef TEST_MODE
#include <sstream>
#include <functional>

TEST_CASE("System Test Suite")
{
    using namespace Extensions::String;
    std::stringstream ss {};

    SECTION("System::Setup")
    {
        System::Setup(DefaultContext, ss, ss);

        CHECK(std::cout.rdbuf() == ss.rdbuf());
        CHECK(std::cerr.rdbuf() == ss.rdbuf());
    }

    SECTION("System::LogTest")
    {
        SECTION("System::LogTest::Log")
        {
            LOG("Testing Testing...");
            CHECK(ss.str() == "Testing Testing...\n");
        }

        SECTION("System::LogTest::LogInternal")
        {
            LOGD("Testing Testing...");
            CHECK(ss.str() == Concat({"[JASM::Log](JASM/src/extensions/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
        }

        SECTION("System::LogTest::LogWarning")
        {
            LOGW("Testing Testing...");
            CHECK(ss.str() == Concat({"[JASM::Warning](JASM/src/extensions/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
        }

        SECTION("System::LogTest::LogError")
        {
            SECTION("System::LogTest::LogError::Normal")
            {
                LOGE(System::LogLevel::Normal, "Testing Testing...");
                CHECK(ss.str() == Concat({"[JASM::Error](JASM/src/extensions/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
            }

            SECTION("System::LogTest::LogError::Low")
            {
                LOGE(System::LogLevel::Low, "Testing Testing...");
                CHECK(ss.str() == Concat({"ERROR [JASM::Error](JASM/src/extensions/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
            }

            SECTION("System::LogTest::LogError::Medium")
            {
                LOGE(System::LogLevel::Medium, "Testing Testing...");
                CHECK(ss.str() == Concat({"IMPORTANT ERROR [JASM::Error](JASM/src/extensions/system.cpp:", std::to_string(__LINE__-1), ") >>> Testing Testing...\n"}));
            }

            SECTION("System::LogTest::LogError::High")
            {
                CHECK_THROWS_WITH(
                    LOGE(System::LogLevel::High, "Testing Testing..."),
                    "Testing Testing..."
                );

                CHECK(ss.str() == Concat({"ALERT JASM/src/extensions/system.cpp:", std::to_string(__LINE__-4), "\n"}));
            }
        }
    }
}
#endif
