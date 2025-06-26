#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

#include "JASMConfig.hpp"

namespace ByteAssembler
{
    /*
     * I could've made all of this without object orientation 
     * but with the existence of a shared state (AssemblyContext 
     * and AssemblyInfo) I thought it would be easier to design
     * the program with it. So here it is.
     *
     * Plus I might have a pure-virtual base assembler class
     * and have different kinds of assemblers (for machine code etc.)
     */


    // Because of serialization ease, I didn't want to 
    // create enums so just plain ol' namespaces and constexprs
    namespace AssemblyFlags
    {
        inline constexpr uchar_t Shared       = 1;
        inline constexpr uchar_t Static       = 2;
        inline constexpr uchar_t Executable   = 4;
        inline constexpr uchar_t SymbolInfo   = 8;
        inline constexpr uchar_t StoreName    = 16; // For possible future debugging
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
            uchar_t flags;
            DefinedSymbolCollection definedSymbols;
            UnknownSymbolCollection unknownSymbols;
            ImportCollection runtimeImports;
            SymbolMap symbolMap;

        private:
            mutable std::string _infStr = "";

        public:
            AssemblyInfo() = delete;
            AssemblyInfo(const std::string& path, uchar_t flags);
            void Serialize(std::ostream& outFile) const;
            void Deserialize(std::istream& inFile);
            void PrintAssemblyInfo() const;

            void AddSymbol(std::string symbolName, systembit_t address);
            void AddUnknownSymbol(size_t symbolName, systembit_t address);
    };

    using AssemblyInfoCollection = std::vector<AssemblyInfo>;

    class ByteAssembler
    {
        public:
            ByteAssembler() = default; 
            AssemblyInfoCollection Assemble();

        private:
            AssemblyInfo AssembleLibrary(const std::filesystem::path& file);
            AssemblyInfo AssembleExecutable(const std::filesystem::path& file);
            AssemblyInfo& AssembleCommon(AssemblyInfo& assemblyInfo, std::istream& sourceFile, std::ostream& outFile);
    };
}

