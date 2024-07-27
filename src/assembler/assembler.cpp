#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "JASMConfig.hpp"
#include "extensions/system.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "assembler/assembler.hpp"
#include "assembler/instructions.hpp"
#include "assemblycontext.hpp"

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
};


//
// Assembler Implementation
//
std::vector<AssemblyInfo> Assembler::Assemble()
{
    System::Context.PrintContext();

    std::vector<AssemblyInfo> outputVector { };

    // TODO: Loop through all input files and call specified assembler . 
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

AssemblyInfo& Assembler::AssembleCommon(AssemblyInfo& assemblyInfo, std::istream& sourceFile, std::ostream& outFile)
{
    std::string token { Stream::Tokenize(sourceFile) };

    while (token != ".body")
    {
        // The prep instructions here
        if (token == "imp")
        {
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

AssemblyInfo Assembler::AssembleExecutable(const std::string& file)
{
    // Setup
    std::string outPath { file };
    outPath.append(".jef.jo");
    char outFlags { AssemblyFlags::Executable | AssemblyFlags::SymbolInfo };

    if (std::filesystem::exists(outPath))
        std::filesystem::remove(outPath);
    
    std::string directories { outPath.substr(0, outPath.find_last_of('/') + 1) };

    if (!directories.empty())
        std::filesystem::create_directories(directories);

    AssemblyInfo assemblyInfo {
        outPath,
        outFlags
    };

    // Executable byte-code format:
    // First byte    = Endianness flag
    // Next 32 bytes = Entry Point
    // Next 32 bytes = Stack Size
    // Next 32 bytes = Max Heap Size
    // Serialized AssemblyInfo
    // Byte code

    // Process
    // TODO: Read the first section of assembly containing max heap size, stack size and entry point.
    // TODO: The rest is the same as assembling a library.
    std::ifstream sourceFile { file };
    std::ofstream outFile { outPath, std::ios_base::binary };

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

    assemblyInfo.Serialize(outFile);

    sourceFile.close();
    outFile.close();
    return std::move(assemblyInfo);
}

AssemblyInfo Assembler::AssembleLibrary(const std::string& file)
{
    std::string outPath { file };
    outPath.append(".stc.jo");
    char outFlags { AssemblyFlags::Static | AssemblyFlags::SymbolInfo };

    if (std::filesystem::exists(outPath))
        std::filesystem::remove(outPath);
    
    std::string directories { outPath.substr(0, outPath.find_last_of('/') + 1) };

    if (!directories.empty())
        std::filesystem::create_directories(directories);

    AssemblyInfo assemblyInfo {
        outPath,
        outFlags
    };

    // Process
    std::ifstream sourceFile { file };
    std::ofstream outFile { outPath, std::ios_base::binary };

    while (Stream::Tokenize(sourceFile) != ".prep") {}

    //AssembleCommon(assemblyInfo, sourceFile, outFile);

    assemblyInfo.Serialize(outFile);
    
    //sourceFile.close();
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
    if (symbolMap.contains(symbolName))
        LOGW("A symbol with name '", symbolName, "' already exists.");

    symbolMap[symbolName] = address;
    definedSymbols.push_back(std::make_unique<std::string>(symbolMap.find(symbolName)->first));
}

void AssemblyInfo::AddUnknownSymbol(std::string symbolName, systembit_t address)
{
    unknownSymbols.push_back({symbolName, address});
}

void AssemblyInfo::Serialize(std::ostream& outFile)
{
    if (outFile.fail() || outFile.bad())
        LOGE(System::LogLevel::High, "Couldn't open file while serializing assembly info");

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
    Serialization::SerializeContainer<AssemblyInfo::ImportCollection, systembit_t, std::string>(
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
    Serialization::SerializeContainer<DefinedSymbolCollection, systembit_t, KeyReference>(
        definedSymbols, 
        outFile,
        [this](const KeyReference& symbolName, std::ostream& out){
            Serialization::SerializeContainer<std::string, uint16_t, char>(
                *symbolName.get(), 
                out,
                &Serialization::SerializeInteger<char>
            ); 

            Serialization::SerializeInteger(this->symbolMap.at(*symbolName.get()), out);
        }
    );

    // Unknown Symbols
    Serialization::SerializeContainer<UnknownSymbolCollection, systembit_t, SymbolInfo>(
        unknownSymbols, 
        outFile,
        [](const SymbolInfo& symbolInfo, std::ostream& out){
            Serialization::SerializeContainer<std::string, uint16_t, char>(
                symbolInfo.SymbolName, 
                out,
                &Serialization::SerializeInteger<char> 
            ); 
            Serialization::SerializeInteger(symbolInfo.Address, out);
        }
    );
}

void AssemblyInfo::Deserialize(std::istream& inFile)
{
    if (inFile.fail() || inFile.bad())
        LOGE(System::LogLevel::High, "Couldn't open file while deserializing assembly info");

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
    Serialization::DeserializeContainer<DefinedSymbolCollection, systembit_t, KeyReference>(
        definedSymbols, 
        inFile,
        [this](KeyReference& info, std::istream& in) {
            std::string name;
            systembit_t addr;
            Serialization::DeserializeContainer<std::string, uint16_t, char>(
                name, 
                in,
                Serialization::DeserializeInteger<char>
            );
            Serialization::DeserializeInteger(addr, in);

            this->symbolMap[name] = addr;
            info = std::make_unique<std::string>(this->symbolMap.find(name)->first);
        } 
    );

    // Unknown Symbols
    Serialization::DeserializeContainer<UnknownSymbolCollection, systembit_t, SymbolInfo>(
        unknownSymbols, 
        inFile,
        [](SymbolInfo& symbolInfo, std::istream& in) {
            Serialization::DeserializeContainer<std::string, uint16_t, char>(
                symbolInfo.SymbolName,
                in,
                Serialization::DeserializeInteger<char>
            );
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
        std::cout << "\n\t" << *info.get() << " at address " << symbolMap.at(*info.get());

    std::cout << "\nUnknown Symbols [ordered](" << unknownSymbols.size() << "):";

    for (const auto& info : unknownSymbols)
        std::cout << "\n\t" << info.SymbolName << " at address " << info.Address;

    std::cout << "\nRuntime Imports (" << runtimeImports.size() << "):";

    for (const auto& info : runtimeImports)
        std::cout << "\n\t" << info;

    std::cout << '\n';
}