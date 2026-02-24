#pragma once

#include "JASMConfig.hpp"
#include <ostream>

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
            ByteDisassembler(const AssemblyContext&& context);
#endif

            void Disassemble();
            void Disassemble(std::istream& in);
            void Disassemble(std::istream& in, std::ostream& out);

        private:
#ifdef TOOLCHAIN_MODE
            const AssemblyContext context;
#endif
    };
}

