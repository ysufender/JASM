#include <filesystem>
#include <fstream>
#include <algorithm>
#include <string>

#include "catch2/catch_test_macros.hpp"

#include "bytemode/assembler/assembler.hpp"
#include "bytemode/assembler/instructions.hpp"
#include "bytemode/assembler/modeflags.hpp"
#include "catch2/internal/catch_source_line_info.hpp"
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

        namespace op = Instructions::OpCodes;
        namespace reg = ModeFlags::RegisterModeFlags;

        constexpr char desiredBytes[] {
            op::sti, 0x0F, 0x00, 0x00, 0x00,
            op::ldi,
            op::movr, reg::eax, reg::ebx,
            op::rdi,
            op::movc, 0x0F, 0x00, 0x00, 0x00, reg::eax,
            op::addf,
            op::addrf, reg::ebx, reg::ecx,
            op::addsb,
            op::hcp,
            op::scp,
            op::rcp, 0x0A
        };
        char resultBytes[sizeof(desiredBytes)] { };

        using BAsm = typename ByteAssembler::ByteAssembler;
        BAsm assembler;
        BAsm::AssemblyInfoCollection collection = assembler.Assemble();
        std::filesystem::path resPath { System::Context.InputFiles().at(0) };
        resPath.concat(".jo");
        std::ifstream resultStream { resPath, std::ios::binary};

        resultStream.read(resultBytes, sizeof(desiredBytes));

        // To see the differences in case of fail
        for (int i = 0; i < sizeof(desiredBytes); i++)
            CHECK((int)desiredBytes[i] == (int)resultBytes[i]);
    }
}
