// This file includes the C API definitions
// in case of TOOLCHAIN_MODE

#include "JASMConfig.hpp"
#include "bytemode/linker/linker.hpp"
#include "jasm.hpp"
#include "system.hpp"
#include <cstdio>

#ifndef TOOLCHAIN_MODE
int main(int argc, char** args)
{
    return jasmmain(argc, args);
}
#else

#include "bytemode/assembler/assembler.hpp"
#include "assemblycontext.hpp"

namespace JASM {
extern "C"
{
    JASMAssemblyContext CreateAssemblyContext(
        int silent,
        int single, 
        int pipelines,
        const Str out, 
        LibTypes libT, 
        const Str workingDir,
        StrVector const in,
        StrVector const libs,
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

    JASMInfo JASMGetBuildInfo()
    {
        return {
            JASM_VERSION,
            JASM_DESCRIPTION,
            JASM_VERSION_MAJOR,
            JASM_VERSION_MINOR,
            JASM_VERSION_PATCH,
#ifdef USE_PIPELINES_OPT
            true
#else
            false
#endif
        };
    }

    size_t HashString(Str str)
    {
        return Extensions::String::Hash(str);
    }

    void DeleteAssemblyContext(JASMAssemblyContext context)
    {
        delete static_cast<AssemblyContext*>(context.ptr);
    }

    JASMByteAssembler CreateByteAssembler(JASMAssemblyContext context)
    {
        ByteAssembler::ByteAssembler* basm = new ByteAssembler::ByteAssembler { *static_cast<AssemblyContext*>(context.ptr) };
        return { basm };
    }

    void DeleteByteAssembler(JASMByteAssembler assembler)
    {
        delete static_cast<ByteAssembler::ByteAssembler*>(assembler.ptr);
    }

    JASMAssemblyInfoCollection ByteAssemble(JASMByteAssembler assembler)
    {
        try
        {
            ByteAssembler::ByteAssembler& basm { *static_cast<ByteAssembler::ByteAssembler*>(assembler.ptr) };
            ByteAssembler::AssemblyInfoCollection* asminfs = new ByteAssembler::AssemblyInfoCollection { basm.Assemble() };
            return JASMAssemblyInfoCollection { asminfs };
        }
        catch (const JASMException& exc)
        {
            static ByteAssembler::AssemblyInfoCollection empty { };
            LOGE(System::LogLevel::Medium, exc.Stringify());
            return JASMAssemblyInfoCollection { &empty };
        }
    }

    JASMByteLinker CreateByteLinker()
    {
        return { new ByteLinker::ByteLinker { } };
    }

    void DeleteByteLinker(JASMByteLinker linker)
    {
        delete static_cast<ByteLinker::ByteLinker*>(linker.ptr);
    }

    ErrorTypes ByteLink(JASMByteLinker linker, JASMAssemblyInfoCollection objects, JASMAssemblyContext context)
    {
        if (static_cast<ByteAssembler::AssemblyInfoCollection*>(objects.ptr)->empty())
            return AssemblerError;

        try
        {
            ByteLinker::ByteLinker& blink { *static_cast<ByteLinker::ByteLinker*>(linker.ptr) };
            ByteAssembler::AssemblyInfoCollection& col { *static_cast<ByteAssembler::AssemblyInfoCollection*>(objects.ptr) };
            AssemblyContext& ctx { *static_cast<AssemblyContext*>(context.ptr) };

            blink.Link(col, ctx);
            return Ok;
        }
        catch (const JASMException& exc)
        {
            LOGE(System::LogLevel::Medium, exc.Stringify());
            return LinkerError;
        }
    }
}}
#endif
