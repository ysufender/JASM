#pragma once

#include "assemblycontext.hpp"

class AssemblerOutput
{
    public:
        const std::string path;

        // TODO: Symbol Informations for functions
        // TODO: Symbol Informations for variables

    public:
        AssemblerOutput(const std::string outPath);
};

using asmout_t = std::vector<AssemblerOutput>;

class Assembler
{
    private:
        const AssemblyContext& context;

    public:
        Assembler(const AssemblyContext& assemblyContext) : context(assemblyContext) { }
        asmout_t Assemble();

    private:
        AssemblerOutput AssembleLibrary(const std::string& file);
        AssemblerOutput AssembleExecutable(const std::string& file);
};