#pragma once

void PrintHeader();
void PrintHelp();

#ifndef NDEBUG
#include "assemblycontext.hpp"
void Finalize(const AssemblyContext& context);
#endif