#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "JASMConfig.hpp"
#include "assemblycontext.hpp"

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
        throw std::runtime_error { "At least one input file is needed." };
        return;
    }

    inputFiles = in;
    isLib = (libT == "shd" || libT == "stc");
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

    ss << "." << (isLib ? libType : "jef");
    outFile = ss.str();
}

const bool& AssemblyContext::IsSilent() const { return silentMode; }
const bool& AssemblyContext::IsSingle() const { return singleAssembly; }
const std::string& AssemblyContext::OutFile() const { return outFile; }
const bool& AssemblyContext::IsLib() const { return isLib; }
const std::string& AssemblyContext::LibType() const { return libType; }
const std::vector<std::string>& AssemblyContext::InputFiles() const { return inputFiles; }
const std::vector<std::string>& AssemblyContext::Libraries() const { return libraries; }

void AssemblyContext::PrintContext(std::ostream& out) const 
{
    if (silentMode)
        return;

    out << "JASM Version " << JASM_VERSION << " Assembly Context";
    out << "\n\tSingle Mode: " << (singleAssembly ? "Enabled" : "Disabled");
    if (!singleAssembly) { out << "\n\tOutput File: " << outFile; }
    if (isLib) { out << "\n\tLibrary Type: " << (libType == "shd" ? "Shared" : "Static" ); }

    out << "\n\tInput Files: " << inputFiles.size() << " total {";
    for (const std::string& file : inputFiles)
    {
        out << "\n\t\t\"" << file << '\"';
        
        if (inputFiles.back() != file)
            out << ',';
    }
    out << "\n\t}";
    
    if(libraries.size() != 0)
    {
        out << "\n\tLibraries: " << libraries.size() << " total {";
        for (const std::string& lib : libraries) 
        {
            out << "\n\t\t\"" << lib << '\"';
        
            if (libraries.back() != lib)
                out << ',';
        }
        out << "\n\t}";
    }
    else
        out << "\n\tLibraries: No Library Provided";
}