#pragma oncejasm

#include <cstdint>


void PrintHeader();
void PrintHelp();

#ifndef NDEBUG
#include "assembler/assembler.hpp"

void Finalize(const Assembler::AssemblyInfoCollection& context);
#endif