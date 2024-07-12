#include <exception>
#include <iostream>

#include "main.hpp"
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
        parser.AddFlag("single", FlagType::Bool);       // Assemble files but do NOT link
        parser.AddFlag("out", FlagType::String);        // Place the output file to specified path if flag `single` is not set
        parser.AddFlag("libType", FlagType::String);    // If desired output is a library, specify the type. (either shared or static)
        parser.AddFlag("in", FlagType::StringList);     // Files to assemble and (optionally) link
        parser.AddFlag("libs", FlagType::StringList);   // Both static and shared libraries to link
        Flags flags = parser.Parse();

        if (argc == 1 || flags.GetBool("help"))
            PrintHelp();
        else if (flags.GetBool("version"))
            std::cout << "JASM Version " << JASM_VERSION << '\n';
        else
        {
            const AssemblyContext assemblyContext {
                flags.GetBool("single"),
                flags.GetString("out"),
                flags.GetString("libType"),
                flags.GetStringList("in"),
                flags.GetStringList("libs")
            };

            Assembler assembler { assemblyContext };
            assembler.Assemble();
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "An error occured during process."
                  << "\n\tProvided information: " << exception.what() << std::endl;
    }


    return 1;
}

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
    -help: Print help.
    -version: Print version.

    -single: Assemble each file provided by `-in` but do NOT link.
    -out: Place the output file to specified path if flag `-single` is not set.
    -libType <lib_type>: If desired output is a library, specify the type. (either shared 'shd' or static 'stc')

    -in <..input..>: Files to assemble and (optionally) link. The first entry is treated as main file containing entry point.
    -libs <..libraries..>: Libraries used and to be linked.
    )";
}