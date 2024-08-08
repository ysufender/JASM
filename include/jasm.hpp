#pragma once

#include "CLIParser.hpp"

void PrintHeader() noexcept;
void PrintHelp(const CLIParser::Flags& flags) noexcept;

CLIParser::Flags SetUpCLI(char** args, int argc);
