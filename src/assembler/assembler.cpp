#include <cctype>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include "JASMConfig.hpp"
#include "utilities/system.hpp"
#include "utilities/stringextensions.hpp"
#include "utilities/streamextensions.hpp"
#include "assembler/assembler.hpp"
#include "assemblycontext.hpp"


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

    while (Extensions::Stream::Tokenize(sourceFile) != ".prep") {}

    // 
    // Set the origin point
    //
    std::string token { Extensions::Stream::Tokenize(sourceFile) };
    if (token != "org")
        LOGE("Expected org after .prep", System::LogLevel::High);
    // origin set block. Not a part of the if block
    { 
        std::string entryName { Extensions::Stream::Tokenize(sourceFile) };
        systembit_t address { 0 }; 
        outFile.seekp(0, std::ios_base::beg);
        outFile.write(reinterpret_cast<char*>(&address), sizeof(address));
        assemblyInfo.unknownSymbols.push_back( {entryName, 0} );
    }
    
    //
    // Set the stack size
    //
    token = Extensions::Stream::Tokenize(sourceFile);
    if (token != "sts")
        LOGE("Expected sts after org", System::LogLevel::High);
    // Stack size set block. Not a part of the if block
    {
        std::string sizeStr { Extensions::Stream::Tokenize(sourceFile) };
        systembit_t size { static_cast<systembit_t>(std::stoul(sizeStr)) };

        outFile.seekp(4, std::ios_base::beg);
        outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
    }

    //
    // Set max heap size
    //
    token = Extensions::Stream::Tokenize(sourceFile);
    if (token != "sth")
        LOGE("Expected sth after sts", System::LogLevel::High);
    // Heap size set block. Not a part of the if block
    {
        std::string sizeStr { Extensions::Stream::Tokenize(sourceFile) };
        systembit_t size { static_cast<systembit_t>(std::stoul(sizeStr)) };

        outFile.seekp(8, std::ios_base::beg);
        outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
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
        throw std::runtime_error{"Couldn't open file while serializing assembly info"};

    // Write the assembly flags
    outFile.write(&flags, sizeof(flags));

    // Store File Name if Flag is Set
    if (flags & AssemblyFlags::StoreName)
    {
        size_t size { path.size() };
        outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
        outFile.write(path.data(), size);
    }
    
    if (!(flags & AssemblyFlags::SymbolInfo))
    {
        outFile.close();
        return;
    }

    // Write symbol count
    systembit_t sizeSym = static_cast<systembit_t>(definedSymbols.size());
    outFile.write(reinterpret_cast<char*>(&sizeSym), sizeof(sizeSym));
    // Iterate through symbols and write both size and path and info.Address
    for(systembit_t i = 0; i < sizeSym; i++)
    {
        SymbolInfo& info { definedSymbols.at(i) };
        uint16_t nameSize { static_cast<uint16_t>(info.SymbolName.size()) };
        outFile.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        outFile.write(info.SymbolName.data(), nameSize);
        outFile.write(reinterpret_cast<char*>(&info.Address), sizeof(info.Address));
    }

    // Write unknown symbol count
    sizeSym = static_cast<systembit_t>(unknownSymbols.size());
    outFile.write(reinterpret_cast<char*>(&sizeSym), sizeof(sizeSym));
    // Iterate through symbols and write both size and path and info.Address
    for (systembit_t i = 0; i < sizeSym; i++)
    {
        SymbolInfo& info { unknownSymbols.at(i) };
        uint16_t nameSize { static_cast<uint16_t>(info.SymbolName.size()) };
        outFile.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        outFile.write(info.SymbolName.data(), nameSize);
        outFile.write(reinterpret_cast<char*>(&info.Address), sizeof(info.Address));
    }
}

void AssemblyInfo::Deserialize(std::ifstream& inFile)
{
    if (inFile.fail())
        throw std::runtime_error{"Couldn't open file while deserializing assembly info"};

    // Read the assembly flags
    inFile.read(&flags, sizeof(flags));
    
    // Read the File Name if Flag is Set
    if (flags & AssemblyFlags::StoreName)
    {
        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
        path.resize(size);
        inFile.read(path.data(), size);
    }

    if (!(flags & AssemblyFlags::SymbolInfo))
    {
        inFile.close();
        return;
    }

    // Read symbol count
    systembit_t sizeSym;
    inFile.read(reinterpret_cast<char*>(&sizeSym), sizeof(sizeSym));
    // Read symbols
    definedSymbols.resize(sizeSym);
    definedSymbols.clear();
    while (sizeSym > 0)
    {
        SymbolInfo info;

        // Read the SymbolName
        uint16_t nameSize;
        inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        info.SymbolName.resize(nameSize);
        inFile.read(info.SymbolName.data(), nameSize);

        // Read the Address
        inFile.read(reinterpret_cast<char*>(&info.Address), sizeof(info.Address));

        definedSymbols.push_back(std::move(info));

        sizeSym--;
    }


    // Read unknown symbol count
    inFile.read(reinterpret_cast<char*>(&sizeSym), sizeof(sizeSym));
    // Read Symbols
    unknownSymbols.resize(sizeSym);
    unknownSymbols.clear();
    while (sizeSym > 0)
    {
        SymbolInfo info;
        
        // Read the SymbolName
        uint16_t nameSize;
        inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        info.SymbolName.resize(nameSize);
        inFile.read(info.SymbolName.data(), nameSize);

        // Read the Address
        inFile.read(reinterpret_cast<char*>(&info.Address), sizeof(info.Address));

        unknownSymbols.push_back(std::move(info));

        sizeSym--;
    }
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