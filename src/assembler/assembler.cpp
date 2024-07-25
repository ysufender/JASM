#include <cctype>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include "JASMConfig.hpp"
#include "utilities/system.hpp"
#include "utilities/stringextensions.hpp"
#include "utilities/serialization.hpp"
#include "utilities/streamextensions.hpp"
#include "assembler/assembler.hpp"
#include "assemblycontext.hpp"

using namespace Extensions;


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

AssemblyInfo& Assembler::AssembleCommon(AssemblyInfo& assemblyInfo, std::ifstream& sourceFile, std::ofstream& outFile)
{
    return assemblyInfo;
}

AssemblyInfo Assembler::AssembleExecutable(const std::string& file)
{
    // Setup
    std::string outPath { file };
    outPath.append(".jef");
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
        LOGE("An Error Occured While Reading the Source File", System::LogLevel::High);
    }

    while (Stream::Tokenize(sourceFile) != ".prep") {}

    // 
    // Set the origin point
    //
    std::string token { Stream::Tokenize(sourceFile) };
    if (token != "org")
        LOGE("Expected org after .prep", System::LogLevel::High);
    // origin set block. Not a part of the if block
    { 
        std::string entryName { Stream::Tokenize(sourceFile) };
        Serialization::SerializeInteger(0, outFile);
        assemblyInfo.unknownSymbols.push_back( {entryName, 0} );
    }
    
    //
    // Set the stack size
    //
    token = Stream::Tokenize(sourceFile);
    if (token != "sts")
        LOGE("Expected sts after org", System::LogLevel::High);
    // Stack size set block. Not a part of the if block
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
        LOGE("Expected sth after sts", System::LogLevel::High);
    // Heap size set block. Not a part of the if block
    {
        std::string sizeStr { Stream::Tokenize(sourceFile) };
        systembit_t size { static_cast<systembit_t>(std::stoul(sizeStr)) };
        Serialization::SerializeInteger(size, outFile);
    }

    AssembleCommon(assemblyInfo, sourceFile, outFile);

    sourceFile.close();
    outFile.close();
    return std::move(assemblyInfo);
}

AssemblyInfo Assembler::AssembleLibrary(const std::string& file)
{
    std::string outPath { file };
    outPath.append(".stc");
    char outFlags { AssemblyFlags::Static | AssemblyFlags::SymbolInfo };

    if (std::filesystem::exists(outPath))
        std::filesystem::remove(outPath);
    
    std::string directories { outPath.substr(0, outPath.find_last_of('/') + 1) };

    if (!directories.empty())
        std::filesystem::create_directories(directories);

#ifndef NDEBUG
    outFlags |= AssemblyFlags::StoreName;
#endif

    AssemblyInfo assemblyInfo {
        outPath,
        outFlags
    };

    // Process
    //std::ifstream sourceFile { file };
    std::ofstream outFile { outPath, std::ios_base::binary };

#ifndef NDEBUG
        assemblyInfo.Serialize(outFile);
#endif
    
    //sourceFile.close();
    outFile.close();
    return std::move(assemblyInfo);
}

//
// AssemblyInfo Implementation 
//
AssemblyInfo::AssemblyInfo(const std::string& path, char flags) : path(path), flags(flags)
{
}

void AssemblyInfo::Serialize(std::ofstream& outFile)
{
    if (outFile.fail() || outFile.bad())
        LOGE("Couldn't open file while serializing assembly info", System::LogLevel::High);

    // Flags
    Serialization::SerializeInteger(flags, outFile);

    if (flags & AssemblyFlags::StoreName)
        Serialization::SerializeContainer<std::string, size_t, char>(
                path, 
                &Serialization::SerializeInteger<char>, 
                outFile
        );

    if (!(flags & AssemblyFlags::SymbolInfo))
        return;
    
    // Known Symbols
    Serialization::SerializeContainer<std::vector<SymbolInfo>, systembit_t, SymbolInfo>(
            definedSymbols, 
            [](const SymbolInfo& symInfo, std::ofstream& out){
                Serialization::SerializeContainer<std::string, uint16_t, char>(
                        symInfo.SymbolName, 
                        &Serialization::SerializeInteger<char>, 
                        out
                ); 
                Serialization::SerializeInteger(symInfo.Address, out);
            },
            outFile
    );

    // Unknown Symbols
    Serialization::SerializeContainer<std::vector<SymbolInfo>, systembit_t, SymbolInfo>(
            unknownSymbols, 
            [](const SymbolInfo& symInfo, std::ofstream& out){
                Serialization::SerializeContainer<std::string, uint16_t, char>(
                        symInfo.SymbolName, 
                        &Serialization::SerializeInteger<char>, 
                        out
                ); 
                Serialization::SerializeInteger(symInfo.Address, out);
            },
            outFile
    );
}

void AssemblyInfo::Deserialize(std::ifstream& inFile)
{
    if (inFile.fail() || inFile.bad())
        LOGE("Couldn't open file while deserializing assembly info", System::LogLevel::High);

    Serialization::DeserializeInteger(flags, inFile);

    if (flags & AssemblyFlags::StoreName)
        Serialization::DeserializeContainer<std::string, size_t, char>(
                path, 
                Serialization::DeserializeInteger<char>, 
                inFile
        );

    if (!(flags & AssemblyFlags::SymbolInfo))
        return;

    // Defined Symbols
    Serialization::DeserializeContainer<std::vector<SymbolInfo>, systembit_t, SymbolInfo>(
            definedSymbols, 
            [](SymbolInfo& info, std::ifstream& in) {
                Serialization::DeserializeContainer<std::string, uint16_t, char>(
                    info.SymbolName, 
                    Serialization::DeserializeInteger<char>, 
                    in
                );

                Serialization::DeserializeInteger(info.Address, in);
            }, 
            inFile
    );

    // Unknown Symbols
    Serialization::DeserializeContainer<std::vector<SymbolInfo>, systembit_t, SymbolInfo>(
            unknownSymbols, 
            [](SymbolInfo& info, std::ifstream& in) {
                Serialization::DeserializeContainer<std::string, uint16_t, char>(
                    info.SymbolName, 
                    Serialization::DeserializeInteger<char>, 
                    in
                );

                Serialization::DeserializeInteger(info.Address, in);
            }, 
            inFile
    );
}

void AssemblyInfo::PrintAssemblyInfo() const
{
    std::cout << "\n\nInfo " << (path.empty() ? "Unknown" : path) 
              << "\nFlags: " << static_cast<int>(flags)
              << "\nDefined Symbols [ordered](" << definedSymbols.size() << "):";

    for (const auto& info : definedSymbols)
        std::cout << "\n\t" << info.SymbolName << " at address " << info.Address;

    std::cout << "\nUnknown Symbols [ordered](" << unknownSymbols.size() << "):";

    for (const auto& info : unknownSymbols)
        std::cout << "\n\t" << info.SymbolName << " at address " << info.Address;
}