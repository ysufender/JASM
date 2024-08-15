#include "catch2/catch_test_macros.hpp"

#include "system.hpp"
#include <fstream>
#include <algorithm>
#include "assembler/byteassembler/assembler.hpp"

TEST_CASE("Assembler Tests")
{
    System::Setup(
        AssemblyContext {
            true, 
            true, 
            true,
            "out.stc", 
            "stc", 
            "asm/",
            { "test.jasm" }, 
            { }
        },
        std::cout,
        std::cerr
    );

    SECTION("Basic Output Test")
    {
        char resultBytes[13] { };
        constexpr char desiredOutput[] {
            0x01, 0x0F, 0x00, 0x00, 0x00,
                0x07,
                0x10, 0x09, 0x08,
                0x0A,

                0x0E, 0x0F, 0x08
        };

        using BAsm = typename ByteAssembler::ByteAssembler;
        BAsm assembler;
        BAsm::AssemblyInfoCollection collection = assembler.Assemble();

        std::ifstream resultStream { System::Context.InputFiles().at(0), std::ios::binary};

        resultStream.read(resultBytes, 13);
        CHECK(std::ranges::equal(resultBytes, desiredOutput));
    }
}
