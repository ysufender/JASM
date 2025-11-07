#include <filesystem>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

#include "JASMConfig.hpp"
#include "assemblycontext.hpp"

#include "system.hpp"

AssemblyContext::AssemblyContext(
        bool silent,
        bool single, 
        bool pipelines,
        const std::string& out, 
#ifndef TOOLCHAIN_MODE
        const std::string& libT, 
#else
        LibTypeEnum libT,
#endif
        const std::filesystem::path& workingDir,
        const std::vector<std::string>& in,
        const std::vector<std::string>& libs,
        bool storeSymbols,
        bool storeName
    ) : _pipelines(pipelines), _silentMode(silent), _singleAssembly(single), _contextString(""), _symbolInfo(storeSymbols), _storeName(storeName)
{
    if (in.size() == 0)
    {
        throw JASM_ERR("At least one input file is needed.");
        return;
    }

    _inputFiles = in;
#ifndef TOOLCHAIN_MODE
    _isLib = !libT.empty();
    _libType = (_isLib ? (libT == "shd" ? LibTypeEnum::Shared : (libT == "stc" ? LibTypeEnum::Static : LibTypeEnum::Executable)) : LibTypeEnum::Executable);
#else
    _isLib = (libT != LibTypeEnum::Executable);
    _libType = libT;
#endif

    _libraries = libs;

    if (_libType == LibTypeEnum::Executable && _isLib)
    {
        LOGW("An unsupported library type has been provided. The assembler will proceed with executable format.");
        _isLib = false;
    }

    _workingDir = (workingDir.empty() ? std::filesystem::current_path() : workingDir);

    if (!std::filesystem::exists(_workingDir) || !std::filesystem::is_directory(_workingDir))
        LOGE(System::LogLevel::High, "Given directory '", _workingDir.generic_string(), "' does not exists. It can't be used as working directory.");

    std::stringstream ss;

    // Input file extensions are '.jasm' for files
    if (out.empty())
    {
        std::string copy { _inputFiles.at(0) };
        ss << copy.erase(copy.size()-5, 5);
    }
    else
        ss << out;

#ifndef TOOLCHAIN_MODE
    std::string extension { _isLib ? '.'+libT : ".jef" };
#else
    std::string extension;
    if (libT == LibTypeEnum::Static)
        extension = ".stc";
    else if (libT == LibTypeEnum::Shared)
        extension = ".shd";
    else
        extension = ".jef";
#endif
    if (std::filesystem::path(ss.str()).extension() != extension)
        ss << extension;
    _outFile = ss.str();
}

bool AssemblyContext::IsSilent() const { return _silentMode; }
bool AssemblyContext::IsSingle() const { return _singleAssembly; }
bool AssemblyContext::IsUsingPipelines() const { return _pipelines; }
bool AssemblyContext::IsLib() const { return _isLib; }
bool AssemblyContext::StoreSymbols() const { return _symbolInfo; }
bool AssemblyContext::StoreName() const { return _storeName; }
std::string_view AssemblyContext::OutFile() const { return _outFile; }
LibTypeEnum AssemblyContext::LibType() const { return _libType; }
const std::filesystem::path& AssemblyContext::WorkingDir() const { return _workingDir; }
const std::vector<std::string>& AssemblyContext::InputFiles() const { return _inputFiles; }
const std::vector<std::string>& AssemblyContext::Libraries() const { return _libraries; }

void AssemblyContext::PrintContext() const 
{
    if (_silentMode)
        return;

    if (_contextString.empty())
    {
        std::stringstream ss;

        ss << "JASM Version " << JASM_VERSION << " Assembly Context"
            << "\n\tSingle Mode: " << (_singleAssembly ? "Enabled" : "Disabled")
            << "\n\tPipelines: " << (_pipelines ? "Enabled" : "Disabled")
            << "\n\tWorking Directory: " << _workingDir;

        if (!_singleAssembly) { ss << "\n\tOutput File: " << _outFile; }
        if (_isLib) { ss << "\n\tLibrary Type: " << (_libType == LibTypeEnum::Shared ? "Shared" : "Static" ); }

        ss << "\n\tInput Files: " << _inputFiles.size() << " total {";
        for (const std::string& file : _inputFiles)
        {
            ss << "\n\t\t\"" << file << '\"';

            if (_inputFiles.back() != file)
                ss << ',';
        }
        ss << "\n\t}";

        if(_libraries.size() == 0)
            ss << "\n\tLibraries: No Library Provided\n\n";
        else
        {
            ss << "\n\tLibraries: " << _libraries.size() << " total {";
            for (const std::string& lib : _libraries) 
            {
                ss << "\n\t\t\"" << lib << '\"';

                if (_libraries.back() != lib)
                    ss << ',';
            }
            ss << "\n\t}\n\n";
        }
        _contextString = std::move(ss.str());
    }

    std::cout << _contextString;
}
