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
        const std::string& out, 
        const std::string& libT, 
        const std::vector<std::string>& in,
        const std::vector<std::string>& libs
    ) : silentMode(silent), singleAssembly(single)
{
    if (in.size() == 0)
    {
        throw JASM_ERR("At least one input file is needed.");
        return;
    }

    inputFiles = in;
    isLib = (libT == "shd" || libT == "stc");
    libType = (isLib ? (libT == "shd" ? LibTypeEnum::Shared : LibTypeEnum::Static) : LibTypeEnum::Executable);
    libraries = libs;

    std::stringstream ss;

    // Input file extensions are '.jasm' for files
    if (out.empty())
    {
        std::string copy { inputFiles.at(0) };
        ss << copy.erase(copy.size()-5, 5);
    }
    else
        ss << out;

    ss << "." << (isLib ? libT : "jef");
    outFile = ss.str();
}

const bool& AssemblyContext::IsSilent() const { return silentMode; }
const bool& AssemblyContext::IsSingle() const { return singleAssembly; }
const std::string& AssemblyContext::OutFile() const { return outFile; }
const bool& AssemblyContext::IsLib() const { return isLib; }
LibTypeEnum AssemblyContext::LibType() const { return libType; }
const std::vector<std::string>& AssemblyContext::InputFiles() const { return inputFiles; }
const std::vector<std::string>& AssemblyContext::Libraries() const { return libraries; }

void AssemblyContext::PrintContext() const 
{
    if (silentMode)
        return;

    std::cout << "JASM Version " << JASM_VERSION << " Assembly Context"
              << "\n\tSingle Mode: " << (singleAssembly ? "Enabled" : "Disabled");

    if (!singleAssembly) { std::cout << "\n\tOutput File: " << outFile; }
    if (isLib) { std::cout << "\n\tLibrary Type: " << (libType == LibTypeEnum::Shared ? "Shared" : "Static" ); }

    std::cout << "\n\tInput Files: " << inputFiles.size() << " total {";
    for (const std::string& file : inputFiles)
    {
        std::cout << "\n\t\t\"" << file << '\"';
        
        if (inputFiles.back() != file)
            std::cout << ',';
    }
    std::cout << "\n\t}";
    
    if(libraries.size() == 0)
    {
        std::cout << "\n\tLibraries: No Library Provided\n";
        return;
    }

    std::cout << "\n\tLibraries: " << libraries.size() << " total {";
    for (const std::string& lib : libraries) 
    {
        std::cout << "\n\t\t\"" << lib << '\"';

        if (libraries.back() != lib)
            std::cout << ',';
    }
    std::cout << "\n\t}\n";
}
