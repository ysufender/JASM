#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>

#include "jasm.hpp"
#include "CLIParser.hpp"
#include "JASMConfig.hpp"

#ifdef TEST_MODE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN 
#endif

#include "test/test.hpp"

#ifndef TEST_MODE
#include <exception>

#include "extensions/system.hpp"
#include "assembler/assembler.hpp"
#include "assemblycontext.hpp"

int main(int argc, char** args)
{
    using namespace CLIParser;

    try
    {
        Parser parser{args, argc, "--", "-"};
        parser.AddFlag("help", FlagType::Bool);
        parser.AddFlag("version", FlagType::Bool);
        parser.AddFlag("silent", FlagType::Bool);
        parser.AddFlag("single", FlagType::Bool);       // Assemble files but do NOT link
        parser.AddFlag("out", FlagType::String);        // Place the output file to specified path if flag `single` is not set
        parser.AddFlag("libType", FlagType::String);    // If desired output is a library, specify the type. (either shared or static)
        parser.AddFlag("in", FlagType::StringList);     // Files to assemble and (optionally) link
        parser.AddFlag("libs", FlagType::StringList);   // Both static and shared libraries to link
        parser.AddFlag("pipelines", FlagType::Bool);

        parser.BindFlag("p", "pipelines");
        parser.BindFlag("h", "help");
        parser.BindFlag("v", "version");
        parser.BindFlag("s", "silent");
        parser.BindFlag("S", "single");
        parser.BindFlag("o", "out");
        parser.BindFlag("i", "in");
        parser.BindFlag("l", "libs");

        CLIParser::Flags flags = parser.Parse();

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
#endif

void PrintHeader() noexcept
{
    std::cout << "\nJust an Assembler (JASM)"
              << "\n\tDescription: " << JASM_DESCRIPTION
              << "\n\tVersion: " << JASM_VERSION
              << "\n\tUse Pipelines: "
#ifdef USE_PIPELINES_OPT
              << "Available"
#else
              << "Unavailable"
#endif
              << "\n\tBuild Mode: "
#ifdef TOOLCHAIN_MODE
              << "Toolchain"
#else
              << "CLI"
#endif
              << '\n';
}

void PrintHelp() noexcept
{
    PrintHeader();

    std::cout << '\n' << R"(Available Flags:
    --help, -h: Print help.
    --version, -v: Print version.

    --silent, -s : Disables SOME outputs. So it's less painful to look at the screen.
    --single, -S: Assemble each file provided by `-in` but do NOT link.
    --out, -o: Place the output file to specified path if flag `-single` is not set.
    --libType <lib_type>: If desired output is a library, specify the type. (either shared 'shd' or static 'stc')

    --in <..input..>, -I <..input..>: Files to assemble and (optionally) link. The first entry is treated as jasm file containing entry point.
    --libs <..libs..>, -L <..libs..>: Libraries used and to be linked.

    WARNING:
        In single mode, each file will be assembled as a static library. Otherwise the output will be decided by `--libType` flag.
    )";
}
