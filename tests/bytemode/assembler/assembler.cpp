#include <filesystem>
#include <fstream>
#include <algorithm>
#include <string>

#include "catch2/catch_test_macros.hpp"

#include "bytemode/assembler/assembler.hpp"
#include "system.hpp"

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


    SECTION("Simple Output Test")
    {
        CHECK(System::Context.IsLib());

        char resultBytes[13] { };
        constexpr char desiredOutput[] {
            0x01, 0x0F, 0x00, 0x00, 0x00,
            0x07,
            0x10, 0x08, 0x09,
            0x0A,
            0x0E, 0x0F, 0x00, 0x00, 0x00, 0x08 
        };

        using BAsm = typename ByteAssembler::ByteAssembler;
        BAsm assembler;
        BAsm::AssemblyInfoCollection collection = assembler.Assemble();
        std::ifstream resultStream { System::Context.InputFiles().at(0), std::ios::binary};

        resultStream.read(resultBytes, 13);
        for (int i = 0; i < 13; i++)
            CHECK(resultBytes[i] == desiredOutput[i]);
    }
}
