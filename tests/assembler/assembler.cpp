#include "catch2/catch_test_macros.hpp"

#include "extensions/system.hpp"
#include "assembler/assembler.hpp"

TEST_CASE("Assembler Tests")
{
    AssemblyContext context {
        true, 
        true, 
        "testOut.stc", 
        "stc", 
        { "script/test.jasm" }, 
        { }
    };

    System::Setup(context);

    constexpr char desiredOutput[] {
         
    };
}
