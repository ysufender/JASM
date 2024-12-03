#pragma once

#include "bytemode/assembler/assembler.hpp"


namespace ByteLinker
{
    class ByteLinker
    {
        public:
            ByteLinker() = default;
            ByteAssembler::AssemblyInfo Link(const ByteAssembler::AssemblyInfoCollection& objects);
    };
}
