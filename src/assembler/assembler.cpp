#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "assembler/assembler.hpp"
#include "assemblycontext.hpp"


//
// Assembler Implementation
//
std::vector<AssemblyInfo> Assembler::Assemble()
{
    context.PrintContext(std::cout);

    std::vector<AssemblyInfo> outputVector { };

    // TODO: Loop through all input files and call specified assembler function. 
    const std::vector<std::string>& inFiles { context.InputFiles() };
    size_t i { 0 };

    if (!context.IsLib() && !context.IsSingle())
    {
        outputVector.push_back(AssembleExecutable(inFiles[0]));
        i = 1;
    }

    for (; i < inFiles.size(); i++)
    {
        const std::string& inputFile { inFiles[i] };
        outputVector.push_back(AssembleLibrary(inputFile));
    }

    std::cout << "\nDone.\n";
    return std::move(outputVector);
}

AssemblyInfo Assembler::AssembleExecutable(const std::string& file)
{
    std::string outPath { file };
    outPath.append(".jef");
    char outFlags { AssemblyFlags::Executable | AssemblyFlags::SymbolInfo };

    AssemblyInfo assemblyInfo {
        outPath,
        outFlags
    };
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

    if (context.IsSingle())
    {
        std::ofstream out { outPath, std::ios::binary };
        assemblyInfo.Serialize(out);
        out.close();
    }
    
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
    if (outFile.fail())
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

    // Write function symbol count
    symaddr_t sizeSym = static_cast<symaddr_t>(functionSymbols.size());
    outFile.write(reinterpret_cast<char*>(&sizeSym), sizeof(sizeSym));
    // Iterate through symbols and write both size and path and address
    for(symaddr_t i = 0; i < sizeSym; i++)
    {
        auto& [name, address] { functionSymbols.at(i) };
        maxnamesize_t nameSize { static_cast<maxnamesize_t>(name.size()) };
        outFile.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        outFile.write(name.data(), nameSize);
        outFile.write(reinterpret_cast<char*>(&address), sizeof(address));
    }

    // Write Variable Symbol Count
    sizeSym = static_cast<symaddr_t>(variableSymbols.size());
    outFile.write(reinterpret_cast<char*>(&sizeSym), sizeof(sizeSym));
    // Iterate through symbols and write both size and path and address
    for (symaddr_t i = 0; i < sizeSym; i++)
    {
        auto& [name, info] { variableSymbols.at(i) };
        maxnamesize_t nameSize { static_cast<maxnamesize_t>(name.size()) };
        outFile.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        outFile.write(name.data(), nameSize);
        char t { info.first };
        symaddr_t a { info.second };
        outFile.write(&t, sizeof(t));
        outFile.write(reinterpret_cast<char*>(&a), sizeof(a));
    }


    outFile.close();
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

    // Read function symbol count
    symaddr_t sizeSym;
    inFile.read(reinterpret_cast<char*>(&sizeSym), sizeof(sizeSym));
    // Read symbols
    functionSymbols.resize(sizeSym);
    functionSymbols.clear();
    while (sizeSym > 0)
    {
        maxnamesize_t nameSize;
        symname_t name;
        inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        name.resize(nameSize);
        inFile.read(name.data(), nameSize);
        symaddr_t address;
        inFile.read(reinterpret_cast<char*>(&address), sizeof(address));
        functionSymbols.push_back({std::move(name), address});

        sizeSym--;
    }


    // Read variable symbol count
    inFile.read(reinterpret_cast<char*>(&sizeSym), sizeof(sizeSym));
    // Read Symbols
    variableSymbols.resize(sizeSym);
    variableSymbols.clear();
    while (sizeSym > 0)
    {
        maxnamesize_t nameSize;
        symname_t  name;
        inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        name.resize(nameSize);
        inFile.read(name.data(), nameSize);
       
        int varType;
        inFile.read(reinterpret_cast<char*>(&varType), sizeof(varType));
        symaddr_t address;
        inFile.read(reinterpret_cast<char*>(&address), sizeof(address));
        variableSymbols.push_back({name, {varType, address}});

        sizeSym--;
    }


    inFile.close();
}

void AssemblyInfo::PrintAssemblyInfo() const
{
    const auto varTypeToString = [](const int& varType) {
        switch (static_cast<int>(varType)) {
            case 0:
                return "Int";
            case 1:
                return "UInt";
            case 2:
                return "Byte";
            case 3:
                return "UByte";
            case 4:
                return "Array";
            default:
                return "Unknown";
        }
    };

    std::cout << "\n\nInfo " << (path.empty() ? "Unknown" : path) 
              << "\nFlags: " << static_cast<int>(flags)
              << "\nFunction Symbols [ordered](" << functionSymbols.size() << "):";

    for (const auto& [name, address] : functionSymbols)
        std::cout << "\n\t" << name << " at address " << address;

    std::cout << "\nVariable Symbols [ordered](" << variableSymbols.size() << "):";

    for (const auto& [name, varInf] : variableSymbols)
        std::cout << "\n\t" << name << " [" << varTypeToString(varInf.first) << "] at address " << varInf.second;
}