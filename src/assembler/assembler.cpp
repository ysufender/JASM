#include "assembler/assembler.hpp"
#include "assemblycontext.hpp"
#include <filesystem>
#include <iostream>
#include <utility>

inline constexpr char _shared       = 0b00000001;
inline constexpr char _static       = 0b00000010;
inline constexpr char _executable   = 0b00000100;

// Returns the assembled libraries to pass to linker.
// First entry is always the executable with entry point, if
// the project is not specified as a library.
asmout_t Assembler::Assemble()
{
    context.PrintContext();

    asmout_t outputVector { };

    // TODO: Loop through all input files and call specified assembler function. 

    if (!context.IsLib())
        outputVector.push_back(AssembleExecutable(context.InputFiles()[0]));

    for (const auto& inputFile : context.InputFiles())
        outputVector.push_back(AssembleLibrary(inputFile));

    return std::move(outputVector);
}

AssemblerOutput Assembler::AssembleExecutable(const std::string& file)
{
    std::string outPath { file };
    outPath.append(".jef");

    AssemblerOutput output { outPath };

    return output;
}

AssemblerOutput Assembler::AssembleLibrary(const std::string& file)
{
    std::string outPath { file };
    outPath.append(".stc");

    AssemblerOutput output { outPath };
    
    return output;
}

AssemblerOutput::AssemblerOutput(const std::string outPath) : path(outPath)
{
    if(std::filesystem::exists(outPath)) 
        std::cout << "\nWARNING: A file with same name at path `" << outPath
                  << "` already exists. The process will overwrite the file.";
}