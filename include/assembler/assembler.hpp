#pragma once

#include <string>
#include <cstdint>

#include "assemblycontext.hpp"


// Because of serialization ease, I didn't want to 
// create enums so just plain namespaces and constexprs
namespace VarType
{
    constexpr char Int = 0;
    constexpr char UInt = 1;
    constexpr char Byte = 2;
    constexpr char UByte = 3;
    constexpr char Array = 4;
};

namespace AssemblyFlags
{
    constexpr char Shared       = 0b00000001;
    constexpr char Static       = 0b00000010;
    constexpr char Executable   = 0b00000100;
    constexpr char SymbolInfo   = 0b00001000;
    constexpr char StoreName    = 0b00010000; // For possible future debugging
};

// POD
struct AssemblyInfo
{
    // Makes tweaking things easier
    using symaddr_t = uint32_t;
    using maxnamesize_t = uint16_t;
    using symname_t = std::string; // size can be max maxnamelen_t
    using syminf_t = std::pair<symname_t, symaddr_t>;
    using varinf_t = std::pair<symname_t, std::pair<char, symaddr_t>>;
    using fnsym_t = std::vector<syminf_t>;
    using varsym_t = std::vector<varinf_t>;

    public:
        std::string path;   // Doesn't get serialized
        char flags;
        fnsym_t functionSymbols;
        varsym_t variableSymbols;

    public:
        AssemblyInfo() = default;
        AssemblyInfo(const std::string& path, char flags);
        void Serialize(std::ofstream& outFile);
        void Deserialize(std::ifstream& inFile);
        void PrintAssemblyInfo() const;
};

class Assembler
{
    private:
        const AssemblyContext& context;

    public:
        Assembler(const AssemblyContext& assemblyContext) : context(assemblyContext) { }
        std::vector<AssemblyInfo> Assemble();

    private:
        AssemblyInfo AssembleLibrary(const std::string& file);
        AssemblyInfo AssembleExecutable(const std::string& file);
};