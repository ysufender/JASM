#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ostream>
#include <string>
#include <string_view>

#include "extensions/system.hpp"
#include "assemblycontext.hpp"

// 
// System Implementation
//
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
            throw JASM_ERR(message.data());
            break;
    }

    std::cerr << "[JASM::Error](" << file.substr(idx, file.size() - idx) << ':' << line << ") >>> " << message << '\n';
}

//
// System::JASMException Implementation
//
JASMException::JASMException(std::string message, std::string file, int line)
    : std::runtime_error(message), _line(line), _message(message)
{
    size_t idx { file.find_first_of("JASM") };
    _file = { file.substr(idx, file.size() - idx) };

    std::stringstream ss;

    ss << message << " [" << _file << ':' << _line << "]\n";
    _fullStr = ss.str();
}

int JASMException::GetLine() const { return _line; }
const std::string& JASMException::GetFile() const { return _file; }
const std::string& JASMException::GetMsg() const { return _message; }

const std::string& JASMException::Stringify() const { return _fullStr; }

std::ostream& operator<<(std::ostream& out, const JASMException& exc)
{
    out << exc.Stringify();
    return out;
}
