#pragma once

#include <fstream>
#include <string>

#include "assemblycontext.hpp"
#include "JASMConfig.hpp"
#include "assembler/instructions.hpp"

/*
* I could've made all of this without object orientation 
* but with the existence of a shared state (AssemblyContext 
* and AssemblyInfo) I thought it would be easier to design
* the program with it. So here it is.
*/


// Because of serialization ease, I didn't want to 
// create enums so just plain ol' namespaces and constexprs
namespace AssemblyFlags
{
    constexpr char Shared       = 1;
    constexpr char Static       = 2;
    constexpr char Executable   = 4;
    constexpr char SymbolInfo   = 8;
    constexpr char StoreName    = 16; // For possible future debugging
};

struct SymbolInfo
{
    std::string SymbolName;
    systembit_t Address;
};

struct AssemblyInfo
{
    using SymbolCollection = std::vector<SymbolInfo>;
    using ImportCollection = std::vector<std::string>;

    std::string path;
    char flags;
    SymbolCollection definedSymbols;
    SymbolCollection unknownSymbols;
    ImportCollection runtimeImports;

#ifndef NDEBUG
    AssemblyInfo() = default;
#else
    AssemblyInfo() = delete;
#endif
    AssemblyInfo(const std::string& path, char flags);
    void Serialize(std::ofstream& outFile);
    void Deserialize(std::ifstream& inFile);
    void PrintAssemblyInfo() const;
};

class Assembler
{
    public:
        using AssemblyInfoCollection = std::vector<AssemblyInfo>;

        Assembler() = default; 
        AssemblyInfoCollection Assemble();

    private:
        AssemblyInfo AssembleLibrary(const std::string& file);
        AssemblyInfo AssembleExecutable(const std::string& file);
        AssemblyInfo& AssembleCommon(AssemblyInfo& assemblyInfo, std::ifstream& sourceFile, std::ofstream& outFile);
};