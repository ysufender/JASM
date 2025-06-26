#pragma once

#include "bytemode/assembler/assembler.hpp"


namespace ByteLinker
{
    class ByteLinker
    {
        public:
            ByteLinker() = default;
            void Link(const ByteAssembler::AssemblyInfoCollection& objects);
            void LinkSingle(const ByteAssembler::AssemblyInfoCollection& objects);
            void LinkLib(const ByteAssembler::AssemblyInfoCollection& objects);
            void LinkExe(const ByteAssembler::AssemblyInfoCollection& objects);
    };
}
