#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "JASMConfig.hpp"

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
    ENUMER char Shared       = 1;
    ENUMER char Static       = 2;
    ENUMER char Executable   = 4;
    ENUMER char SymbolInfo   = 8;
    ENUMER char StoreName    = 16; // For possible future debugging
};

struct SymbolInfo 
{
    size_t SymbolHash;
    systembit_t Address;
};

struct AssemblyInfo
{
    public:
        //using SymbolCollection = std::vector<SymbolInfo>;
        using ImportCollection = std::vector<std::string>;
        using KeyType = size_t;
        using DefinedSymbolCollection = std::vector<KeyType>;
        using UnknownSymbolCollection = std::vector<SymbolInfo>;
        using SymbolMap = std::unordered_map<size_t, systembit_t>;

        std::string path;
        char flags;
        DefinedSymbolCollection definedSymbols;
        UnknownSymbolCollection unknownSymbols;
        ImportCollection runtimeImports;
        SymbolMap symbolMap;

    public:
        AssemblyInfo() = delete;
        AssemblyInfo(const std::string& path, char flags);
        void Serialize(std::ostream& outFile);
        void Deserialize(std::istream& inFile);
        void PrintAssemblyInfo() const;

        void AddSymbol(std::string symbolName, systembit_t address);
        void AddUnknownSymbol(std::string symbolName, systembit_t address);
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
        AssemblyInfo& AssembleCommon(AssemblyInfo& assemblyInfo, std::istream& sourceFile, std::ostream& outFile);
};
