#include <filesystem>
#include <iostream>
#include <fstream>

#include "assembler/assembler.hpp"
#include "assemblycontext.hpp"


//
// Assembler Implementation
//
Assembler::asminf_t Assembler::Assemble()
{
    context.PrintContext();

    Assembler::asminf_t outputVector { };

    // TODO: Loop through all input files and call specified assembler function. 

    if (!context.IsLib() && !context.IsSingle())
        outputVector.push_back(AssembleExecutable(context.InputFiles()[0]));

    for (const auto& inputFile : context.InputFiles())
        outputVector.push_back(AssembleLibrary(inputFile));

    std::cout << "\nDone.\n";
    return std::move(outputVector);
}

AssemblyInfo Assembler::AssembleExecutable(const std::string& file)
{
    std::string outPath { file };
    outPath.append(".jef");

    AssemblyInfo assemblyInfo;
    
    std::cout << "\n\tDone.";
    return std::move(assemblyInfo);
}

AssemblyInfo Assembler::AssembleLibrary(const std::string& file)
{
    std::string outPath { file };
    outPath.append(".stc");

    if (std::filesystem::exists(outPath))
        std::filesystem::remove(outPath);

    AssemblyInfo assemblyInfo;
    assemblyInfo.path = outPath;
    assemblyInfo.flags = AssemblyFlags::Static | AssemblyFlags::SymbolInfo;

    if (context.IsSingle())
    {
        std::ofstream out { outPath, std::ios::binary };
        assemblyInfo.Serialize(out);
    }
    
    return std::move(assemblyInfo);
}

//
// AssemblyInfo Implementation 
//
void AssemblyInfo::Serialize(std::ofstream& outFile)
{
    // Write the assembly flags
    outFile.write(&flags, sizeof(flags));

    // Write the path size and path
    size_t size { path.size() };
    outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
    outFile.write(path.data(), size);

    // Write function symbol count
    size = functionSymbols.size();
    outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
    // Iterate through symbols and write both size and path and address
    for(size_t i = 0; i < size; i++)
    {
        auto& [name, address] { functionSymbols.at(i) };
        size_t nameSize { name.size() };
        outFile.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        outFile.write(name.data(), nameSize);
        outFile.write(reinterpret_cast<char*>(&address), sizeof(address));
    }

    // Write Variable Symbol Count
    size = variableSymbols.size();
    outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
    // Iterate through symbols and write both size and path and address
    for (size_t i = 0; i < size; i++)
    {
        auto& [name, info] { variableSymbols.at(i) };
        size_t nameSize { name.size() };
        outFile.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        outFile.write(name.data(), nameSize);
        int t { info.first };
        symaddr_t a { info.second };
        outFile.write(reinterpret_cast<char*>(&t), sizeof(info.first));
        outFile.write(reinterpret_cast<char*>(&a), sizeof(info.second));
    }


    outFile.close();
}

void AssemblyInfo::Deserialize(std::ifstream& inFile)
{
    // Read the assembly flags
    inFile.read(&flags, sizeof(flags));

    // Read the path
    size_t size;
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
    path.clear();
    path.resize(size);
    inFile.read(path.data(), size);

    // Read function symbol count
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
    // Read symbols
    functionSymbols.resize(size);
    functionSymbols.clear();
    while (size > 0)
    {
        size_t nameSize;
        symname_t name;
        inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        name.resize(nameSize);
        inFile.read(name.data(), nameSize);
        symaddr_t address;
        inFile.read(reinterpret_cast<char*>(&address), sizeof(address));
        functionSymbols.push_back({std::move(name), address});

        size--;
    }


    // Read variable symbol count
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
    // Read Symbols
    variableSymbols.resize(size);
    variableSymbols.clear();
    while (size > 0)
    {
        size_t nameSize;
        symname_t  name;
        inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        name.resize(nameSize);
        inFile.read(name.data(), nameSize);
       
        int varType;
        inFile.read(reinterpret_cast<char*>(&varType), sizeof(varType));
        symaddr_t address;
        inFile.read(reinterpret_cast<char*>(&address), sizeof(address));
        variableSymbols.push_back({name, {varType, address}});

        size--;
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

    std::cout << "\n\nInfo \"" << path << "\"" 
              << "\nFlags: " << static_cast<int>(flags)
              << "\nFunction Symbols [ordered](" << functionSymbols.size() << "):";

    for (const auto& [name, address] : functionSymbols)
        std::cout << "\n\t" << name << " at address " << address;

    std::cout << "\nVariable Symbols [ordered](" << variableSymbols.size() << "):";

    for (const auto& [name, varInf] : variableSymbols)
        std::cout << "\n\t" << name << " [" << varTypeToString(varInf.first) << "] at address " << varInf.second;
}