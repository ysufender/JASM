#pragma once

#include <string>

#include "utilities/stringextensions.hpp"
#include "assemblycontext.hpp"

#ifndef NDEBUG
    #define LOGD(...) LOG(Extensions::String::Concat({__VA_ARGS__}))
#else
    #define LOGD(message)
#endif
#define LOG(...) System::Log(Extensions::String::Concat({__VA_ARGS__}), __FILE__, __LINE__)
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

    static void Log(const std::string& message, const std::string& file, int line);
    static void LogWarning(const std::string& message, const std::string& file, int line);
    static void LogError(const std::string& message, LogLevel level, const std::string& file, int line);
    static void Setup(const AssemblyContext& context);
};