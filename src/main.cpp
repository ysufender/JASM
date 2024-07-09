#include <iostream>

#include "main.hpp"
#include "CLIParser.hpp"
#include "JASMConfig.hpp"
#include "assembler/assembler.hpp"

int main(int argc, char** args)
{
    CLIParser parser{args, argc};
    parser.AddFlag("help", FlagType::Bool);
    parser.AddFlag("version", FlagType::Bool);

    parser.AddFlag("single", FlagType::Bool);       // Assemble files but do NOT link
    parser.AddFlag("out", FlagType::String);        // Place the output file to specified path if flag `single` is not set

    parser.AddFlag("lib", FlagType::String);        // If desired output is a library, specify the type. (either shared or static)
    parser.AddFlag("in", FlagType::StringList);     // Files to assemble and (optionally) link

    Flags flags = parser.Parse();

    if (argc == 1 || flags.GetBool("help"))
        printHelp();
    else if (flags.GetBool("version"))
        std::cout << "JASM Version " << JASM_VERSION << '\n';

    return 1;
}

void printHeader()
{
    std::cout << "Just an Assembler (JASM)\n";
    std::cout << "\tDescription: " << JASM_DESCRIPTION << '\n';
    std::cout << "\tVersion: " << JASM_VERSION << '\n';
}

void printHelp()
{
    printHeader();
}