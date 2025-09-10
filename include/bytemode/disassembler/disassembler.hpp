#pragma once

#include <filesystem>

#include "JASMConfig.hpp"
#include "bytemode/assembler/assembler.hpp"

#ifdef TOOLCHAIN_MODE
#include "assemblycontext.hpp"
#endif

namespace ByteAssembler
{
    class ByteDisassembler
    {
        public:
#ifndef TOOLCHAIN_MODE
            ByteDisassembler() = default; 
#else
            ByteAssembler(const AssemblyContext&& context);
#endif

            void Disassemble();

        private:
#ifdef TOOLCHAIN_MODE
            const AssemblyContext context;
#endif
    };
}

