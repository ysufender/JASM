#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "system.hpp"
#include "JASMConfig.hpp"
#include "assemblycontext.hpp"
#include "extensions/stringextensions.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "assembler/byteassembler/assembler.hpp"
#include "assembler/byteassembler/instructions.hpp"

namespace ByteAssembler
{
    //
    // Usings
    //
    using namespace Extensions;

    //
    // Map for instruction handler callbacks.
    // It's better than a bunch of if-else statements
    // I think
    //
    static const std::unordered_map<std::string, std::function<void(AssemblyInfo&, std::istream&, std::ostream&)>> instructionMap {
        {"nop", &Instructions::Nop},
            {"stc", &Instructions::StoreConstant},
            {"ldc", &Instructions::LoadConstant}, 
            {"rda", &Instructions::ReadAddress},
            {"mov", &Instructions::Move },
            {"add", &Instructions::Add },
            {"ads", &Instructions::AddSafe },
    };


    //
    // Assembler Implementation
    //
    std::vector<AssemblyInfo> ByteAssembler::Assemble()
    {
        System::Context.PrintContext();

        std::vector<AssemblyInfo> outputVector { };

        const std::vector<std::string>& inFiles { System::Context.InputFiles() };
        size_t i { 0 };

        if (!System::Context.IsLib() && !System::Context.IsSingle())
        {
            outputVector.push_back(AssembleExecutable(inFiles[0]));
            i = 1;
        }

        for (; i < inFiles.size(); i++)
        {
            const std::string& inputFile { inFiles[i] };
            outputVector.push_back(AssembleLibrary(inputFile));
        }

        return std::move(outputVector);
    }

    AssemblyInfo& ByteAssembler::AssembleCommon(AssemblyInfo& assemblyInfo, std::istream& sourceFile, std::ostream& outFile)
    {
        std::string token { Stream::Tokenize(sourceFile) };

        while (token != ".body")
        {
            //
            // The prep instructions here
            //
            if (token == "imp")
            {
                // Letting the VM know that we need the particular shd file.
                token = Stream::Tokenize(sourceFile);
                assemblyInfo.runtimeImports.push_back(token);
            }
            else
                LOGE(
                        System::LogLevel::High,
                        "Unexpected token '", token , "' in prep section."
                    );

            token = Stream::Tokenize(sourceFile);
        }

        // token is now ".body". So we get the next one.
        token = Stream::Tokenize(sourceFile);

        while (token != ".end")
        {
            if (token.back() == ':')
            {
                token.pop_back();
                systembit_t index { static_cast<systembit_t>(outFile.tellp()) }; 

                assemblyInfo.AddSymbol(token, index);
            }
            else if (instructionMap.contains(token))
                instructionMap.at(token)(assemblyInfo, sourceFile, outFile);
            else
                LOGE(System::LogLevel::High, "Couldn't find '", token, "' on inscturcion map.");

            token = Stream::Tokenize(sourceFile);
        }

        return assemblyInfo;
    }

    AssemblyInfo ByteAssembler::AssembleExecutable(const std::filesystem::path& file)
    {
        // Setup
        std::filesystem::path outPath { file };
        outPath.replace_extension(".jo");
        char outFlags { AssemblyFlags::Executable | AssemblyFlags::SymbolInfo };

        if (std::filesystem::exists(outPath))
            std::filesystem::remove(outPath);

        //std::string directories { outPath.substr(0, outPath.find_last_of('/') + 1) };
        //std::filesystem::path directories { outPath.parent_path() };

        //if (!directories.empty())
        //std::filesystem::create_directories(directories);

        AssemblyInfo assemblyInfo {
            outPath,
                outFlags
        };

        std::ifstream sourceFile { System::OpenInFile(file, std::ios::in) };
        std::ofstream outFile { System::OpenOutFile(outPath) };

        if (sourceFile.bad())
        {
            sourceFile.close();
            LOGE(System::LogLevel::High, "An Error Occured While Reading the Source File");
        }

        while (Stream::Tokenize(sourceFile) != ".prep") {}

        // 
        // Set the origin point
        //
        std::string token { Stream::Tokenize(sourceFile) };
        if (token != "org")
            LOGE(System::LogLevel::High, "Expected org after .prep");
        else
        { 
            std::string entryName { Stream::Tokenize(sourceFile) };
            Serialization::SerializeInteger<systembit_t>(0, outFile);
            assemblyInfo.AddUnknownSymbol(entryName, 0);
        }

        //
        // Set the stack size
        //
        token = Stream::Tokenize(sourceFile);
        if (token != "sts")
            LOGE(System::LogLevel::High, "Expected sts after org");
        else
        {
            std::string sizeStr { Stream::Tokenize(sourceFile) };
            systembit_t size { static_cast<systembit_t>(std::stoul(sizeStr)) };
            Serialization::SerializeInteger(size, outFile);
        }

        //
        // Set max heap size
        //
        token = Stream::Tokenize(sourceFile);
        if (token != "sth")
            LOGE(System::LogLevel::High, "Expected sth after sts");
        else
        {
            std::string sizeStr { Stream::Tokenize(sourceFile) };
            systembit_t size { static_cast<systembit_t>(std::stoul(sizeStr)) };
            Serialization::SerializeInteger(size, outFile);
        }

        //AssembleCommon(assemblyInfo, sourceFile, outFile);
        LOGW("AssembleCommon is not called for ", file);

        sourceFile.close();
        outFile.close();
        return std::move(assemblyInfo);
    }

    AssemblyInfo ByteAssembler::AssembleLibrary(const std::filesystem::path& file)
    {
        std::filesystem::path outPath { file };
        outPath.replace_extension(".jo");
        char outFlags { AssemblyFlags::Static | AssemblyFlags::SymbolInfo };

        if (std::filesystem::exists(outPath))
            std::filesystem::remove(outPath);

        //std::string directories { outPath.substr(0, outPath.find_last_of('/') + 1) };
        //std::filesystem::path directories { outPath.parent_path() };

        //if (!directories.empty())
        //std::filesystem::create_directories(directories);

        AssemblyInfo assemblyInfo {
            outPath,
                outFlags
        };

        // Process
        std::ifstream sourceFile { System::OpenInFile(file, std::ios::in) };
        std::ofstream outFile { System::OpenOutFile(outPath) };

        while (Stream::Tokenize(sourceFile) != ".prep") { }

        AssembleCommon(assemblyInfo, sourceFile, outFile);

        // Since the process just ends after this.
        //if (System::Context.IsSingle())
        //assemblyInfo.Serialize(outFile);

        sourceFile.close();
        outFile.close();
        return assemblyInfo;
    }

    //
    // AssemblyInfo Implementation 
    //
    AssemblyInfo::AssemblyInfo(const std::string& path, char flags) : symbolMap(), path(path), flags(flags)
    {
    }

    void AssemblyInfo::AddSymbol(std::string symbolName, systembit_t address)
    {
        size_t symbolHash { String::Hash(symbolName) };

        if (symbolMap.contains(symbolHash))
            LOGW("A symbol with name '", symbolName, "' already exists.");

        symbolMap[symbolHash] = address;
        definedSymbols.push_back(symbolHash);
    }

    void AssemblyInfo::AddUnknownSymbol(std::string symbolName, systembit_t address)
    {
        unknownSymbols.push_back({String::Hash(symbolName), address});
    }

    void AssemblyInfo::Serialize(std::ostream& outFile)
    {
        if (outFile.fail() || outFile.bad())
            LOGE(System::LogLevel::High, "Couldn't open file while serializing assembly info");

        outFile.seekp(0, std::ios::beg); 

        // Format:
        //      flags
        //      name length
        //      name
        //      import count
        //      imports
        //      known symbol count
        //      symbols
        //      unknown symbol count
        //      symbols

        // Flags
        Serialization::SerializeInteger(flags, outFile);

        // Name
        if (flags & AssemblyFlags::StoreName)
            Serialization::SerializeContainer<std::string, size_t, char>(
                    path, 
                    outFile,
                    &Serialization::SerializeInteger<char> 
                    );

        // Imports
        Serialization::SerializeContainer<ImportCollection, systembit_t, std::string>(
                runtimeImports,
                outFile,
                [](const std::string& data, std::ostream& stream){
                Serialization::SerializeContainer<std::string, uint16_t, char>(
                        data,
                        stream,
                        &Serialization::SerializeInteger<char>
                        );
                }
                );

        // Symbol Info
        if (!(flags & AssemblyFlags::SymbolInfo))
            return;

        // Known Symbols
        Serialization::SerializeContainer<DefinedSymbolCollection, systembit_t, KeyType>(
                definedSymbols, 
                outFile,
                [this](KeyType symbolHash, std::ostream& out){
                Serialization::SerializeInteger(symbolHash, out); 
                Serialization::SerializeInteger(this->symbolMap.at(symbolHash), out);
                }
                );

        // Unknown Symbols
        Serialization::SerializeContainer<UnknownSymbolCollection, systembit_t, SymbolInfo>(
                unknownSymbols, 
                outFile,
                [](const SymbolInfo& symbolInfo, std::ostream& out){
                Serialization::SerializeInteger(symbolInfo.SymbolHash, out); 
                Serialization::SerializeInteger(symbolInfo.Address, out);
                }
                );
    }

    void AssemblyInfo::Deserialize(std::istream& inFile)
    {
        if (inFile.fail() || inFile.bad())
            LOGE(System::LogLevel::High, "Couldn't open file while deserializing assembly info");

        inFile.seekg(0, std::ios::beg); 

        // Flags
        Serialization::DeserializeInteger(flags, inFile);

        //Name
        if (flags & AssemblyFlags::StoreName)
            Serialization::DeserializeContainer<std::string, size_t, char>(
                    path, 
                    inFile,
                    Serialization::DeserializeInteger<char>
                    );

        // Imports
        Serialization::DeserializeContainer<AssemblyInfo::ImportCollection, systembit_t, std::string>(
                runtimeImports, 
                inFile,
                [](std::string& data, std::istream& stream){
                Serialization::DeserializeContainer<std::string, uint16_t, char>(
                        data, 
                        stream,
                        &Serialization::DeserializeInteger<char>
                        );
                } 
                );

        // Symbol Info
        if (!(flags & AssemblyFlags::SymbolInfo))
            return;

        // Defined Symbols
        Serialization::DeserializeContainer<DefinedSymbolCollection, systembit_t, KeyType>(
                definedSymbols, 
                inFile,
                [this](KeyType& symbolHash, std::istream& in) {
                systembit_t addr;
                Serialization::DeserializeInteger(symbolHash, in);
                Serialization::DeserializeInteger(addr, in);

                this->symbolMap[symbolHash] = addr;
                } 
                );

        // Unknown Symbols
        Serialization::DeserializeContainer<UnknownSymbolCollection, systembit_t, SymbolInfo>(
                unknownSymbols, 
                inFile,
                [](SymbolInfo& symbolInfo, std::istream& in) {
                Serialization::DeserializeInteger(symbolInfo.SymbolHash,in);
                Serialization::DeserializeInteger(symbolInfo.Address, in);
                }
                );
    }

    void AssemblyInfo::PrintAssemblyInfo() const
    {
        std::cout << "\n\nInfo " << (path.empty() ? "Unknown" : path) 
            << "\nFlags: " << static_cast<int>(flags)
            << "\nDefined Symbols [ordered](" << definedSymbols.size() << "):";

        for (const auto& info : definedSymbols)
            std::cout << "\n\t" << info << " at address " << symbolMap.at(info);

        std::cout << "\nUnknown Symbols [ordered](" << unknownSymbols.size() << "):";

        for (const auto& info : unknownSymbols)
            std::cout << "\n\t" << info.SymbolHash << " at address " << info.Address;

        std::cout << "\nRuntime Imports (" << runtimeImports.size() << "):";

        for (const auto& info : runtimeImports)
            std::cout << "\n\t" << info;

        std::cout << '\n';
    }
}

