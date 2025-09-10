#include "JASMConfig.hpp"

#ifndef TOOLCHAIN_MODE
#include "jasm.hpp"

int main(int argc, char** args)
{
    return jasmmain(argc, args);
}
#else
#include "bytemode/assembler/assembler.hpp"
#include "assemblycontext.hpp"


int test()
{
    ByteAssembler::ByteAssembler basm { 
        AssemblyContext(false, false, true, "", "", "", {}, {}, false, false)
    };
    return 0;
}
#endif
