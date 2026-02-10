#pragma once

#include "JASMConfig.hpp"
#include "bytemode/assembler/assembler.hpp"
#include "extensions/streamextensions.hpp"


namespace ByteLinker
{
    // I don't really know why this class is not fully static.
    // I don't even know why I made a class at the first place.
    // Bear with me for the time being.
    class ByteLinker
    {
        public:

            ByteLinker() = default;
            void Link(
                ByteAssembler::AssemblyInfoCollection& objects
#ifdef TOOLCHAIN_MODE
                , const AssemblyContext& context
#endif
            );

#ifdef TOOLCHAIN_MODE
            std::ostream& Link(
                ByteAssembler::AssemblyInfoCollection& objects,
                const AssemblyContext& context,
                std::ostream& outFile
            )
#endif

#ifndef TOOLCHAIN_MODE
        private:
#endif
            void LinkSingle(ByteAssembler::AssemblyInfoCollection& objects);
            void LinkSingle(ByteAssembler::AssemblyInfo& info, std::ostream& outFile);

            void LinkLib(ByteAssembler::AssemblyInfoCollection& objects);
            void LinkLib(ByteAssembler::AssemblyInfoCollection& objects, std::ostream& outFile);

            void LinkExe(ByteAssembler::AssemblyInfoCollection& objects);
            void LinkExe(ByteAssembler::AssemblyInfoCollection& objects, std::ostream& outFile);
    };
}
