#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "JASMConfig.hpp"
#include "assemblycontext.hpp"

#include "extensions/system.hpp"

AssemblyContext::AssemblyContext(
        bool silent,
        bool single, 
        bool pipelines,
        const std::string& out, 
        const std::string& libT, 
        const std::string& workingDir,
        const std::vector<std::string>& in,
        const std::vector<std::string>& libs
    ) : _pipelines(pipelines), _workingDir(workingDir), _silentMode(silent), _singleAssembly(single), _contextString("")
{
    if (in.size() == 0)
    {
        throw JASM_ERR("At least one input file is needed.");
        return;
    }

    _inputFiles = in;
    _isLib = (libT == "shd" || libT == "stc");
    _libType = (_isLib ? (libT == "shd" ? LibTypeEnum::Shared : LibTypeEnum::Static) : LibTypeEnum::Executable);
    _libraries = libs;

    std::stringstream ss;

    // Input file extensions are '.jasm' for files
    if (out.empty())
    {
        std::string copy { _inputFiles.at(0) };
        ss << copy.erase(copy.size()-5, 5);
    }
    else
        ss << out;

    ss << "." << (_isLib ? libT : "jef");
    _outFile = ss.str();
}

const bool& AssemblyContext::IsSilent() const { return _silentMode; }
const bool& AssemblyContext::IsSingle() const { return _singleAssembly; }
const std::string& AssemblyContext::OutFile() const { return _outFile; }
const bool& AssemblyContext::IsLib() const { return _isLib; }
LibTypeEnum AssemblyContext::LibType() const { return _libType; }
const std::vector<std::string>& AssemblyContext::InputFiles() const { return _inputFiles; }
const std::vector<std::string>& AssemblyContext::Libraries() const { return _libraries; }

void AssemblyContext::PrintContext() const 
{
    if (_silentMode)
        return;

    if (!_contextString.empty())
        std::cout << _contextString;

    // _contextString prep
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
        {
            ss << "\n\tLibraries: No Library Provided\n";
            return;
        }

        ss << "\n\tLibraries: " << _libraries.size() << " total {";
        for (const std::string& lib : _libraries) 
        {
            ss << "\n\t\t\"" << lib << '\"';

            if (_libraries.back() != lib)
                ss << ',';
        }
        ss << "\n\t}\n";

        _contextString = ss.str();
    }
}
