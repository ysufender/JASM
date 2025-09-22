#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>

#include "JASMConfig.hpp"
#include "assemblycontext.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "system.hpp"
#include "bytemode/linker/linker.hpp"
#include "bytemode/assembler/assembler.hpp"

#define log_ret(...) { LOGW(__VA_ARGS__); return; }

namespace ByteLinker
{
    ByteAssembler::AssemblyInfo::SymbolMap definedSymbols { };
    ByteAssembler::AssemblyInfo::UnknownSymbolCollection unknownSymbols { };
    std::vector<std::string_view> runtimeAssemblies { };

    namespace AsmFlag = ByteAssembler::AssemblyFlags;

    void ByteLinker::Link(const ByteAssembler::AssemblyInfoCollection& objects)
    {
        if (System::Context.IsLib())
            this->LinkLib(objects);
        else if (System::Context.IsSingle())
            this->LinkSingle(objects);
        else
            this->LinkExe(objects);
    }
    

    void ByteLinker::LinkSingle(const ByteAssembler::AssemblyInfoCollection& objects)
    {
        for (const ByteAssembler::AssemblyInfo& info : objects)
        {
            if (info.unknownSymbols.size() != 0)
            {
                LOGE(
                    System::LogLevel::Medium,
                    "Given assembly ", info.path,
                    " contains unknown symbols and can't be turned into a static library."
                );
                continue;
            }

            ByteAssembler::AssemblyInfo newInfo { info };
            newInfo.path.replace(newInfo.path.size()-8, 8, ".stc");

            std::ifstream inFile { System::OpenInFile(info.path) };
            std::ofstream outFile { System::OpenOutFile(newInfo.path) };

            inFile.seekg(0, std::ios::beg);
            while (!inFile.eof())
            {
                char ch;
                Extensions::Serialization::DeserializeInteger(ch, inFile);
                if (inFile.eof())
                    break;
                Extensions::Serialization::SerializeInteger(ch, outFile);
            }

            outFile.seekp(0, std::ios::end);
            OStreamPos(outFile, asmInfoStart);
            newInfo.Serialize(outFile);
            OStreamPos(outFile, asmInfoEnd);
            Extensions::Serialization::SerializeInteger(
                static_cast<uint64_t>(asmInfoEnd-asmInfoStart),
                outFile
            );

            outFile.close();
            newInfo.PrintAssemblyInfo();
        }
    }

    static void LinkStatics(ByteAssembler::AssemblyInfo& final, size_t& currentPos, std::ostream& outFile)
    {
        for (const std::string& libPath : System::Context.Libraries())
        {
            ByteAssembler::AssemblyInfo lib {
                libPath,
                ByteAssembler::AssemblyFlags::Static | ByteAssembler::AssemblyFlags::SymbolInfo | ByteAssembler::AssemblyFlags::StoreName
            };
            std::ifstream inFile { System::OpenInFile(libPath) };
            inFile.seekg(-sizeof(uint64_t), std::ios::end);
            uint64_t size { };
            Extensions::Serialization::DeserializeInteger(size, inFile);
            inFile.seekg(-(sizeof(uint64_t)+size), std::ios::end);
            IStreamPos(inFile, bytecodeEnd);
            lib.Deserialize(inFile);

            for (const std::string& import : lib.runtimeImports)
            {
                runtimeAssemblies.push_back(import); 
                final.runtimeImports.push_back(import);
            }

            for (ByteAssembler::AssemblyInfo::KeyType symbol : lib.definedSymbols)
            {
                if (definedSymbols.contains(symbol))
                    LOGE(System::LogLevel::High, "Redefinition of symbol ", std::to_string(symbol), " in ", lib.path);
                definedSymbols[symbol] = lib.symbolMap.at(symbol) + currentPos;
                final.definedSymbols.push_back(symbol);
                final.symbolMap[symbol] = definedSymbols.at(symbol);
                OStreamPos(outFile, pos);
            }
            for (const auto& unknown : lib.unknownSymbols)
                unknownSymbols.emplace_back(unknown.SymbolHash, unknown.Address + currentPos);

            currentPos += bytecodeEnd;
            inFile.seekg(0, std::ios::beg);
            for (std::streamoff pos = 0; pos != bytecodeEnd; pos++)
            {
                char ch;
                Extensions::Serialization::DeserializeInteger(ch, inFile);
                if (inFile.eof())
                    break;
                Extensions::Serialization::SerializeInteger(ch, outFile);
            }
            inFile.close();
        }
    }

    static void LinkObjects(const ByteAssembler::AssemblyInfo& info, ByteAssembler::AssemblyInfo& final, size_t& currentPos, std::ostream& outFile)
    {
        for (const std::string& import : info.runtimeImports)
        {
            runtimeAssemblies.push_back(import); 
            final.runtimeImports.push_back(import);
        }

        if (!(info.flags & AsmFlag::SymbolInfo))
            log_ret(info.path, " does not contain symbol informations.")

        for (ByteAssembler::AssemblyInfo::KeyType symbol : info.definedSymbols) 
        {
            if (definedSymbols.contains(symbol))
                LOGE(System::LogLevel::High, "Redefinition of symbol ", std::to_string(symbol), " in ", info.path);
            definedSymbols[symbol] = info.symbolMap.at(symbol) + currentPos;
            final.definedSymbols.push_back(symbol);
            final.symbolMap[symbol] = definedSymbols.at(symbol);

            OStreamPos(outFile, pos);
        }
        for (const auto& unknown : info.unknownSymbols)
            unknownSymbols.emplace_back(unknown.SymbolHash, unknown.Address + currentPos);

        std::ifstream inFile { System::OpenInFile(info.path) };
        inFile.seekg(0, std::ios::end);
        IStreamPos(inFile, endPos);
        currentPos += endPos;
        inFile.seekg(0, std::ios::beg);
        while (!inFile.eof())
        {
            char ch;
            Extensions::Serialization::DeserializeInteger(ch, inFile);
            if (inFile.eof())
                break;
            Extensions::Serialization::SerializeInteger(ch, outFile);
        }

        inFile.close();
    }

    static void HandleSymbols(ByteAssembler::AssemblyInfo& final, std::ofstream& outFile)
    {
        for (const auto& [symbol, address] : unknownSymbols)
        {
            if (!definedSymbols.contains(symbol) && runtimeAssemblies.empty())
                LOGE(System::LogLevel::High, "Unknown symbol '", std::to_string(symbol), "' while linking.");
            if (!definedSymbols.contains(symbol))
                LOGE(System::LogLevel::High, "Runtime assemblies are currently not supported. Please statically link your assemblies.");

            OStreamPos(outFile, pos);
 
            outFile.seekp(address, std::ios::beg);
            Extensions::Serialization::SerializeInteger(final.symbolMap.at(symbol), outFile);
        } 

        for (const auto& [symbol, address] : unknownSymbols)
            if (!definedSymbols.contains(symbol) || System::Context.IsLib())
                final.unknownSymbols.emplace_back(symbol, address);
        
        //if (final.flags & ByteAssembler::AssemblyFlags::SymbolInfo)
        //{
        //
        // Every target must include at least basic information about the Assembly.
        outFile.seekp(0, std::ios::end);
        OStreamPos(outFile, asmInfoStart);
        final.Serialize(outFile);
        OStreamPos(outFile, asmInfoEnd);
        Extensions::Serialization::SerializeInteger<uint64_t>(
            asmInfoEnd-asmInfoStart,
            outFile
        );
        //}
        outFile.close();

        final.PrintAssemblyInfo();

        if (final.unknownSymbols.empty())
            return;

        // TODO: Get the AssemblyInfo's of runtimeAssemblies and check
        // if the unknown symbols exist in them or not.
        LOGW("Unknown symbols are present, runtime assemblies are not available.");
        LOGW("TODO: Symbol check with runtime assemblies.");
    }

    void ByteLinker::LinkLib(const ByteAssembler::AssemblyInfoCollection& objects)
    {
        ByteAssembler::AssemblyInfo final {
            System::Context.OutFile().data(),
            static_cast<uchar_t>(
                (System::Context.LibType() == LibTypeEnum::Static ? ByteAssembler::AssemblyFlags::Static : ByteAssembler::AssemblyFlags::Shared) 
                |   ByteAssembler::AssemblyFlags::SymbolInfo
                |   ByteAssembler::AssemblyFlags::StoreName
            ),
        };

        std::ofstream outFile { System::OpenOutFile(System::Context.OutFile()) };
        std::size_t currentPos { 0 };
        
        // Link all object files
        for (const auto& info : objects)
        {
            if ((info.flags & ByteAssembler::AssemblyFlags::Executable))
                LOGE(System::LogLevel::High, "Executable assemblies in library target, aborting. (", info.path, ")");
            LinkObjects(info, final, currentPos, outFile);
        } 

        // Link all static libraries
        LinkStatics(final, currentPos, outFile);
        
        // Handle symbols
        HandleSymbols(final, outFile);
    }

    void ByteLinker::LinkExe(const ByteAssembler::AssemblyInfoCollection& objects)
    {
        ByteAssembler::AssemblyInfo final {
            System::Context.OutFile().data(),
            objects[0].flags
        };

        std::ofstream outFile { System::OpenOutFile(System::Context.OutFile()) };
        std::size_t currentPos { 12 };
        outFile.seekp(12, std::ios::beg);
        
        // Link all object files
        for (const auto& info : objects)
        {
            if (&info == &objects[0])
                continue;
            if (info.flags & ByteAssembler::AssemblyFlags::Executable)
                LOGE(System::LogLevel::High, "Multiple executable targets, aborting. (", info.path, ")");
            LinkObjects(info, final, currentPos, outFile);
        }

        // Link static libraries
        LinkStatics(final, currentPos, outFile); 
        
        // Link the first file lastly, to keep the entry point at the bottom, and ensure the program exits.
        {
            outFile.seekp(0, std::ios::beg);
            std::ifstream inFile { System::OpenInFile(objects[0].path) };
            inFile.seekg(0, std::ios::beg);
            systembit_t data;
            // org
            Extensions::Serialization::DeserializeInteger(data, inFile);
            data += currentPos;
            Extensions::Serialization::SerializeInteger(data, outFile);
            // sts
            Extensions::Serialization::DeserializeInteger(data, inFile);
            Extensions::Serialization::SerializeInteger(data, outFile);
            // sth
            Extensions::Serialization::DeserializeInteger(data, inFile);
            Extensions::Serialization::SerializeInteger(data, outFile);

            for (ByteAssembler::AssemblyInfo::KeyType symbol : objects[0].definedSymbols) 
            {
                if (definedSymbols.contains(symbol))
                    LOGE(System::LogLevel::High, "Redefinition of symbol ", std::to_string(symbol), " in ", objects[0].path);
                definedSymbols[symbol] = objects[0].symbolMap.at(symbol) + currentPos - 12;
                final.definedSymbols.push_back(symbol);
                final.symbolMap[symbol] = definedSymbols.at(symbol);
            }
            for (const auto& unknown : objects[0].unknownSymbols)
                unknownSymbols.emplace_back(unknown.SymbolHash, unknown.Address + ((&unknown != &objects[0].unknownSymbols[0]) ? currentPos - 12 : 0));

            inFile.seekg(0, std::ios::end);
            IStreamPos(inFile, endPos);
            currentPos += endPos - 12;
            inFile.seekg(12, std::ios::beg);
            outFile.seekp(0, std::ios::end);
            while (!inFile.eof())
            {
                char ch;
                Extensions::Serialization::DeserializeInteger(ch, inFile);
                if (inFile.eof())
                    break;
                Extensions::Serialization::SerializeInteger(ch, outFile);
            }

            inFile.close();
        }

        // Handle symbols
        HandleSymbols(final, outFile);
    }
}
