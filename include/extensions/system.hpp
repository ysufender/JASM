#pragma once

#include <iostream>

#include "extensions/stringextensions.hpp"
#include "assemblycontext.hpp"

#ifndef NDEBUG
    #define LOGD(...) System::LogInternal(Extensions::String::Concat({__VA_ARGS__}), __FILE__, __LINE__)
#else
    #define LOGD(message)
#endif
#define LOG(...) System::Log(Extensions::String::Concat({__VA_ARGS__}))
#define LOGW(...) System::LogWarning(Extensions::String::Concat({__VA_ARGS__}), __FILE__, __LINE__)
#define LOGE(level, ...) System::LogError(Extensions::String::Concat({__VA_ARGS__}), level, __FILE__, __LINE__)

struct System
{
    static AssemblyContext& Context; 

    enum class LogLevel
    {
        Normal,
        Low,
        Medium,
        High
    };

    static void Log(std::string_view message);
    static void LogInternal(std::string_view message, std::string_view file, int line);
    static void LogWarning(std::string_view message, std::string_view file, int line);
    static void LogError(std::string_view message, LogLevel level, std::string_view file, int line);
    static void Setup(const AssemblyContext& context, std::ostream& cout = std::cout, std::ostream& cerr = std::cerr);
};
