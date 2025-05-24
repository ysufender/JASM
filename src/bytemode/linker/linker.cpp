#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>

#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "system.hpp"
#include "bytemode/linker/linker.hpp"
#include "bytemode/assembler/assembler.hpp"

#define log_cont(...) { LOGW(__VA_ARGS__); continue; }

namespace ByteLinker
{
    ByteAssembler::AssemblyInfo::SymbolMap definedSymbols { };
    ByteAssembler::AssemblyInfo::UnknownSymbolCollection unknownSymbols { };
    std::vector<std::string_view> runtimeAssemblies { };

    namespace AsmFlag = ByteAssembler::AssemblyFlags;

    ByteAssembler::AssemblyInfo ByteLinker::Link(const ByteAssembler::AssemblyInfoCollection& objects)
    {
        // TODO: Handle runtime assemblies
        // Idea on How to Hande:
        //  1- generate code in place to call runtime assemblies like syscalls, generate fake symbols
        //  2- after sth, list all symbols and their assemblies and their positions, then jmp to org

        std::ofstream outFile { System::OpenOutFile(System::Context.OutFile()) };
        std::size_t currentPos { 0 };
        
        for (const auto& info : objects)
        {
            if ((info.flags & ByteAssembler::AssemblyFlags::Executable) && outFile.tellp() != 0)
                LOGE(System::LogLevel::High, "Multiple executable targets, aborting. (", info.path, ")");

            for (const std::string& import : info.runtimeImports)
                runtimeAssemblies.push_back(import); 

            if (!(info.flags & AsmFlag::SymbolInfo))
                log_cont(info.path, " does not contain symbol informations.")

            for (size_t symbol : info.definedSymbols) 
            {
                if (definedSymbols.contains(symbol))
                    LOGE(System::LogLevel::High, "Redefinition of symbol ", std::to_string(symbol), " in ", info.path);
                definedSymbols[symbol] = info.symbolMap.at(symbol) + currentPos;
            }
            for (const auto& unknown : info.unknownSymbols)
                unknownSymbols.emplace_back(unknown.SymbolHash, unknown.Address + currentPos);

            std::ifstream inFile { System::OpenInFile(info.path) };
            inFile.seekg(0, std::ios::end);
            IStreamPos(inFile, endPos);
            currentPos = endPos;
            inFile.seekg(0, std::ios::beg);
            while ((endPos = inFile.get()) != EOF)
                outFile.write(reinterpret_cast<char*>(&endPos), 1);
        } 

        for (const auto& [symbol, address] : unknownSymbols)
        {
            if (!definedSymbols.contains(symbol) && runtimeAssemblies.empty())
                LOGE(System::LogLevel::High, "Unknown symbol '", std::to_string(symbol), "' while linking.");
            if (!definedSymbols.contains(symbol))
                LOGE(System::LogLevel::High, "Runtime assemblies are currently not supported. Please statically link your assemblies.");

            outFile.seekp(address, std::ios::beg);
            Extensions::Serialization::SerializeInteger(definedSymbols.at(symbol), outFile);
        } 

        return objects[0]; 
    }
}
