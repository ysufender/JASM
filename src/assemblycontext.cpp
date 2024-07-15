#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "JASMConfig.hpp"
#include "assemblycontext.hpp"

AssemblyContext::AssemblyContext(
        bool single, 
        const std::string& out, 
        const std::string& libT, 
        const std::vector<std::string>& in,
        const std::vector<std::string>& libs
    )
{
    if (in.size() == 0)
    {
        throw std::runtime_error { "At least one input file is needed." };
        return;
    }

    singleAssembly = single;
    libType = libT;
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

const bool& AssemblyContext::IsSingle() const { return singleAssembly; }
const std::string& AssemblyContext::OutFile() const { return outFile; }
const bool& AssemblyContext::IsLib() const { return isLib; }
const std::string& AssemblyContext::LibType() const { return libType; }
const std::vector<std::string>& AssemblyContext::InputFiles() const { return inputFiles; }
const std::vector<std::string>& AssemblyContext::Libraries() const { return libraries; }

void AssemblyContext::PrintContext() const 
{
    std::cout << "JASM Version " << JASM_VERSION << " Assembly Context";
    std::cout << "\n\tSingle Mode: " << (singleAssembly ? "Enabled" : "Disabled");
    if (!singleAssembly) { std::cout << "\n\tOutput File: " << outFile; }
    if (isLib) { std::cout << "\n\tLibrary Type: " << (libType == "shd" ? "Shared" : "Static" ); }

    std::cout << "\n\tInput Files: " << inputFiles.size() << " total {";
    for (const std::string& file : inputFiles) { std::cout << file << ", "; }
    std::cout << "\b\b}";
    
    if(libraries.size() != 0)
    {
        std::cout << "\n\tLibraries: " << libraries.size() << " total {";
        for (const std::string& lib : libraries) { std::cout << lib << ", "; }
        std::cout << "\b\b}";   
    }
    else
        std::cout << "\n\tLibraries: No Library Provided";
}