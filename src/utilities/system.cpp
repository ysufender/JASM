#include <iostream>
#include <stdexcept>

#include "utilities/system.hpp"
#include "assemblycontext.hpp"

using systembit_t = uint32_t;

AssemblyContext& System::Context { DefaultContext };

void System::Setup(const AssemblyContext& context)
{
    Context = context;
}

void System::Log(const std::string& message, const std::string& file, int line)
{
    if (Context.IsSilent())
        return;

    size_t idx { file.find_first_of("JASM") };
    std::cout << "[JASM::System::Log](" << file.substr(idx, file.size() - idx) << ':' << line << ") >>> " << message << '\n';
}

void System::LogWarning(const std::string& message, const std::string& file, int line)
{
    if (Context.IsSilent())
        return;

    size_t idx { file.find_first_of("JASM") };
    std::cout << "[JASM::Warning](" << file.substr(idx, file.size() - idx) << ':' << line << ") >>> " << message << '\n';
}

void System::LogError(const std::string& message, LogLevel level, const std::string& file, int line)
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
            throw std::runtime_error(message);
            break;
    }

    std::cerr << "[JASM::Error](" << file.substr(idx, file.size() - idx) << ':' << line << ") >>> " << message << '\n';
}