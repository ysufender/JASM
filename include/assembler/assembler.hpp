#pragma once

#include <string>
#include <cstdint>

#include "assemblycontext.hpp"


// Because of serialization ease, I didn't want to 
// create enums so just plain namespaces and constexprs
namespace VarType
{
    constexpr int Int = 0;
    constexpr int UInt = 1;
    constexpr int Byte = 2;
    constexpr int UByte = 3;
    constexpr int Array = 4;
};

namespace AssemblyFlags
{
    constexpr char Shared       = 0b00000001;
    constexpr char Static       = 0b00000010;
    constexpr char Executable   = 0b00000100;
    constexpr char SymbolInfo   = 0b00001000;
};

// POD
struct AssemblyInfo
{

    using symaddr_t = uint32_t;
    using symname_t = std::string;
    using syminf_t = std::pair<symname_t, symaddr_t>;
    using varinf_t = std::pair<symname_t, std::pair<int, symaddr_t>>;
    using fnsym_t = std::vector<syminf_t>;
    using varsym_t = std::vector<varinf_t>;

    public:
        std::string path;
        char flags;
        fnsym_t functionSymbols;
        varsym_t variableSymbols;

    public:
        void Serialize(std::ofstream& outFile);
        void Deserialize(std::ifstream& inFile);
        void PrintAssemblyInfo() const;
};

class Assembler
{
    using asminf_t = std::vector<AssemblyInfo>;

    private:
        const AssemblyContext& context;

    public:
        Assembler(const AssemblyContext& assemblyContext) : context(assemblyContext) { }
        asminf_t Assemble();

    private:
        AssemblyInfo AssembleLibrary(const std::string& file);
        AssemblyInfo AssembleExecutable(const std::string& file);
};