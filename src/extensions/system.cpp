#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <ostream>
#include <string_view>

#include "extensions/system.hpp"
#include "assemblycontext.hpp"


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
    std::stringstream ss {};

    const auto Test = [](const std::string& signature, std::function<void(std::string_view, std::string_view, int)> logCallback) -> bool 
    {

        logCallback("Testing Testing...", __FILE__, __LINE__);
        bool val = ss.str() == Extensions::String::Concat({
            "[JASM::",
            signature,
            "](JASM/src/extensions/system.cpp:92) >>> Testing Testing...\n"
        });

        ss.str("");
        return val;
    };

    SECTION("System::Setup")
    {
        System::Setup(DefaultContext, ss, ss);

        CHECK(std::cout.rdbuf() == ss.rdbuf());
        CHECK(std::cerr.rdbuf() == ss.rdbuf());
    }

    SECTION("System::LogTest (Info & Warning)")
    {
        LOG("Testing Testing...");
        CHECK(ss.str() == "Testing Testing...\n");
        ss.str("");

        CHECK(Test("Log", System::LogInternal));

        CHECK(Test("Warning", System::LogWarning));

        LOGE(System::LogLevel::Normal, "Testing Testing...");
        CHECK(ss.str() == "[JASM::Error](JASM/src/extensions/system.cpp:135) >>> Testing Testing...\n");
        ss.str("");

        LOGE(System::LogLevel::Low, "Testing Testing...");
        CHECK(ss.str() == "ERROR [JASM::Error](JASM/src/extensions/system.cpp:135) >>> Testing Testing...\n");
        ss.str("");

        LOGE(System::LogLevel::Medium, "Testing Testing...");
        CHECK(ss.str() == "IMPORTANT ERROR [JASM::Error](JASM/src/extensions/system.cpp:135) >>> Testing Testing...\n");
        ss.str("");

        CHECK_THROWS_MESSAGE(
            LOGE(System::LogLevel::Normal, "Testing Testing..."),
            "Testing Testing..."
        );

        System::Setup(DefaultContext, std::cout, std::cerr);
    }
}
#endif
