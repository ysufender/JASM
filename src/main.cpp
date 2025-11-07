// This file includes the C API definitions
// in case of TOOLCHAIN_MODE

#include "JASMConfig.hpp"
#include "bytemode/linker/linker.hpp"
#include "jasm.hpp"

#ifndef TOOLCHAIN_MODE

int main(int argc, char** args)
{
    return jasmmain(argc, args);
}
#else
#include "bytemode/assembler/assembler.hpp"
#include "assemblycontext.hpp"
#include "system.hpp"

namespace JASM
{
    extern "C" JASMAssemblyContext CreateAssemblyContext(
        int silent,
        int single, 
        int pipelines,
        const JASM::Str out, 
        JASM::LibTypeEnum::LibTypes libT, 
        const JASM::Str workingDir,
        JASM::StrVector const in,
        JASM::StrVector const libs,
        int storeSymbols,
        int storeName
    )
    {
        std::vector<std::string> inFiles {in.elements, in.elements+in.size};
        std::vector<std::string> libraries {libs.elements, libs.elements+libs.size};

        AssemblyContext* context = new AssemblyContext{
            static_cast<bool>(silent),
            static_cast<bool>(single),
            static_cast<bool>(pipelines),
            std::string(out),
            static_cast<enum LibTypeEnum>(libT),
            std::string(workingDir),
            inFiles,
            libraries,
            static_cast<bool>(storeSymbols),
            static_cast<bool>(storeName)
        };

        return { context };
    }

    extern "C" JASMByteAssembler CreateByteAssembler(JASMAssemblyContext context)
    {
        ByteAssembler::ByteAssembler* basm = new ByteAssembler::ByteAssembler { *static_cast<AssemblyContext*>(context.ptr) };
        return { basm };
    }

    extern "C" JASMAssemblyInfoCollection ByteAssemble(JASMByteAssembler assembler)
    {
        ByteAssembler::ByteAssembler basm { *static_cast<ByteAssembler::ByteAssembler*>(assembler.ptr) };
        ByteAssembler::AssemblyInfoCollection* asminfs = new ByteAssembler::AssemblyInfoCollection { basm.Assemble() };
        return JASMAssemblyInfoCollection { asminfs };
    }

    extern "C" JASMByteLinker CreateByteLinker()
    {
        return { new ByteLinker::ByteLinker { } };
    }

    extern "C" void ByteLink(JASMByteLinker linker, JASMAssemblyInfoCollection objects, JASMAssemblyContext context)
    {
        ByteLinker::ByteLinker blink { *static_cast<ByteLinker::ByteLinker*>(linker.ptr) };
        blink.Link(
            *static_cast<ByteAssembler::AssemblyInfoCollection*>(objects.ptr),
            *static_cast<AssemblyContext*>(context.ptr)
        );
    }
}
#endif
