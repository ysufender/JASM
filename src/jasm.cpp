#include "assemblycontext.hpp"
#include "utilities/serialization.hpp"
#include "utilities/system.hpp"
#include <cassert>
#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <stdlib.h>
#include <string>

#ifndef NDEBUG
#include <fstream>
#endif

#include "jasm.hpp"
#include "CLIParser.hpp"
#include "JASMConfig.hpp"
#include "assembler/assembler.hpp"

int main(int argc, char** args)
{
    try
    {
        CLIParser parser{args, argc};
        parser.AddFlag("help", FlagType::Bool);
        parser.AddFlag("version", FlagType::Bool);
        parser.AddFlag("silent", FlagType::Bool);
        parser.AddFlag("single", FlagType::Bool);       // Assemble files but do NOT link
        parser.AddFlag("out", FlagType::String);        // Place the output file to specified path if flag `single` is not set
        parser.AddFlag("libType", FlagType::String);    // If desired output is a library, specify the type. (either shared or static)
        parser.AddFlag("in", FlagType::StringList);     // Files to assemble and (optionally) link
        parser.AddFlag("libs", FlagType::StringList);   // Both static and shared libraries to link

        parser.BindFlag("h", "help");
        parser.BindFlag("v", "version");
        parser.BindFlag("s", "silent");
        parser.BindFlag("S", "single");
        parser.BindFlag("o", "out");
        parser.BindFlag("I", "in");
        parser.BindFlag("L", "libs");

        Flags flags = parser.Parse();

        if (argc == 1 || flags.GetBool("help"))
            PrintHelp();
        else if (flags.GetBool("version"))
            std::cout << "JASM Version " << JASM_VERSION << '\n';
        else
        {
            const AssemblyContext assemblyContext {
                flags.GetBool("silent"),
                flags.GetBool("single"),
                flags.GetString("out"),
                flags.GetString("libType"),
                flags.GetStringList("in"),
                flags.GetStringList("libs")
            };

            System::Setup(assemblyContext);

            Assembler assembler;
            auto collection = assembler.Assemble();

#ifndef NDEBUG
            Finalize(collection);
#endif
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "An error occured during process."
                  << "\n\tProvided information: " << exception.what() << std::endl;

        return 1;
    }

    return 0;
}

#ifndef NDEBUG
void Finalize(const Assembler::AssemblyInfoCollection& collection)
{
    std::cout << "\nFinalizing...\n";

    //for (const auto& entry : collection)
    //{
    //    AssemblyInfo info;
    //    std::ifstream inputFile { entry.path, std::ios::binary };
    //    if (entry.flags & AssemblyFlags::Executable)
    //    {
    //        systembit_t entry;
    //        systembit_t stack;
    //        systembit_t heap;
    //        inputFile.read(reinterpret_cast<char*>(&entry), sizeof(entry));
    //        inputFile.read(reinterpret_cast<char*>(&stack), sizeof(stack));
    //        inputFile.read(reinterpret_cast<char*>(&heap), sizeof(heap));
    //        std::cout << "\nProgram Entry Point:      " << entry
    //            << "\nProgram Stack Size:       " << stack
    //            << "\nProgram Max Heap Size:    " << heap;
    //    }
    //    info.Deserialize(inputFile);
    //    info.PrintAssemblyInfo();
    //    inputFile.close();
    //}

    for (const auto& entry : collection)
        entry.PrintAssemblyInfo();
}
#endif

void PrintHeader()
{
    std::cout << "\nJust an Assembler (JASM)\n";
    std::cout << "\tDescription: " << JASM_DESCRIPTION << '\n';
    std::cout << "\tVersion: " << JASM_VERSION << '\n';
}

void PrintHelp()
{
    PrintHeader();

    std::cout << '\n' << R"(Available Flags:
    -help, -h: Print help.
    -version, -v: Print version.

    -silent, -s : Disables SOME outputs. So it's less painful to look at the screen.
    -single, -S: Assemble each file provided by `-in` but do NOT link.
    -out, -o: Place the output file to specified path if flag `-single` is not set.
    -libType <lib_type>: If desired output is a library, specify the type. (either shared 'shd' or static 'stc')

    -in <..input..>, -I <..input..>: Files to assemble and (optionally) link. The first entry is treated as jasm file containing entry point.
    -libs <..libs..>, -L <..libs..>: Libraries used and to be linked.

    WARNING:
        In single mode, each file will be assembled as a static library. Otherwise the output will be decided by `-libType` flag.
    )";
}