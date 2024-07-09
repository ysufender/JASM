#include <iostream>

#include "main.hpp"
#include "CLIParser.hpp"
#include "JASMConfig.hpp"
#include "assembler/assembler.hpp"

int main(int argc, char** args)
{
    std::cout << "Just an ASseMbler (JASM)\n";
    std::cout << "\tDescription: " << JASM_DESCRIPTION << '\n';
    std::cout << "\tVersion: " << JASM_VERSION << '\n';

    printHelp();

    CLIParser parser{args, argc};
    parser.AddFlag("help", FlagType::Bool);
    parser.AddFlag("version", FlagType::Bool);
    parser.AddFlag("S", FlagType::Bool);

    Flags flags = parser.Parse();

    return 1;
}