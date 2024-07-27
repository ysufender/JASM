#include "assemblycontext.hpp"
#include "extensions/serialization.hpp"
#include "extensions/system.hpp"
#include <cassert>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string>

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
        //entry.PrintAssemblyInfo();
    //for (const auto& entry : collection)
    //{
        //std::ifstream in { entry.path, std::ios::binary };
        
        //if (!System::Context.IsSingle() && entry.path == collection.at(0).path)
        //{
            //systembit_t org, ssz, hsz;  
            //Extensions::Serialization::DeserializeInteger(org, in);
            //Extensions::Serialization::DeserializeInteger(ssz, in);
            //Extensions::Serialization::DeserializeInteger(hsz, in);

            //std::cout << "\norg: " << org
                      //<< "\nsts: " << ssz
                      //<< "\nsth: " << hsz << '\n';
        //}

        //AssemblyInfo info {entry.path, 0x00};
        //info.Deserialize(in);
        //info.PrintAssemblyInfo();
    //}
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