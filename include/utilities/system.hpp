#pragma once

#include "assemblycontext.hpp"
#include <string>

#ifndef NDEBUG
    #define LOGD(message) LOG(message)
#else
    #define LOGD(message)
#endif
#define LOG(message) System::Log(message, __FILE__, __LINE__)
#define LOGW(message) System::LogWarning(message, __FILE__, __LINE__)
#define LOGE(message, level) System::LogError(message, level, __FILE__, __LINE__)

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