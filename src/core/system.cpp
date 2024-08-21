#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ostream>
#include <string>
#include <string_view>

#include "JASMConfig.hpp"

#include "CLIParser.hpp"
#include "system.hpp"
#include "assemblycontext.hpp"


// 
// System Implementation
//
AssemblyContext& System::Context { DefaultContext };

void System::Setup(const CLIParser::Flags& flags, std::ostream& cout, std::ostream& cerr)
{
    using FT = CLIParser::FlagType;

    Context = AssemblyContext {
        flags.GetFlag<FT::Bool>("silent"),
        flags.GetFlag<FT::Bool>("single"),
        flags.GetFlag<FT::Bool>("pipelines"),
        flags.GetFlag<FT::String>("out"),
        flags.GetFlag<FT::String>("lib-type"),
        flags.GetFlag<FT::String>("working-dir"),
        flags.GetFlag<FT::StringList>("in"),
        flags.GetFlag<FT::StringList>("libs")
    };
    std::filesystem::current_path(Context.WorkingDir());
    std::cout.rdbuf(cout.rdbuf());
    std::cerr.rdbuf(cerr.rdbuf());
}

#ifdef TEST_MODE
void System::Setup(const AssemblyContext&& context, std::ostream& cout, std::ostream& cerr)
{
    Setup(context, cout, cerr);
}

void System::Setup(const AssemblyContext& context, std::ostream& cout, std::ostream& cerr)
{
    Context = context;
    std::filesystem::current_path(Context.WorkingDir());
    std::cout.rdbuf(cout.rdbuf());
    std::cerr.rdbuf(cerr.rdbuf());
}
#endif

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

std::ifstream System::OpenInFile(const std::filesystem::path& path, const std::ios::openmode mode)
{
    if (!std::filesystem::exists(path))
        LOGE(LogLevel::High, "The file at path '", path.generic_string(), "' does not exist.");

    std::ifstream file { path, mode };

    if (file.fail() || file.bad() || !file.is_open())
        LOGE(LogLevel::High, "An error occured while opening the file '", path.generic_string(), "'.");

    return file;
}

std::ofstream System::OpenOutFile(const std::filesystem::path& path, const std::ios::openmode mode)
{
    if (std::filesystem::exists(path))
        LOGW("A file at path '", path.generic_string(), "' already exists. Overwriting...");

    std::ofstream file { path, mode };

    if (file.fail() || file.bad() || !file.is_open())
        LOGE(LogLevel::High, "An error occured while opening the file '", path.generic_string(), "'.");

    return file;
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
