#include <cstddef>
#include <filesystem>
#include <fstream>

#include "system.hpp"
#include "bytemode/linker/linker.hpp"
#include "bytemode/assembler/assembler.hpp"

#define lret(...) { LOGW(__VA_ARGS__); continue; }

namespace ByteLinker
{
    ByteAssembler::AssemblyInfo::SymbolMap definedSymbols { };
    ByteAssembler::AssemblyInfo::SymbolMap unknownSymbols { };
    std::unordered_map<std::string, ByteAssembler::AssemblyInfo> runtimeAssemblies { };

    namespace AsmFlag = ByteAssembler::AssemblyFlags;

    ByteAssembler::AssemblyInfo ByteLinker::Link(ByteAssembler::AssemblyInfoCollection objects)
    {

        std::ofstream outFile { System::OpenOutFile(System::Context.OutFile()) };
        std::size_t currentPos { 0 };
        
        for (const auto& info : objects)
        {
            if (!(info.flags & AsmFlag::SymbolInfo))
                lret(info.path, " does not contain symbol informations.")
            if (info.flags & AsmFlag::Shared)
            {
            }
        } 
    }
}
