#include <sstream>

#include "bytemode/assembler/assembler.hpp"
#include "catch2/catch_test_macros.hpp"

#include "bytemode/assembler/instructions.hpp"
#include "bytemode/assembler/modeflags.hpp"
#include "system.hpp"


TEST_CASE("Instructions Tests")
{
    System::Setup(
        AssemblyContext {
            true, 
            true, 
            true,
            "", 
            "stc", 
            "",
            { "test.jasm" }, 
            { }
        },
        std::cout,
        std::cerr
    );

    std::stringstream in;
    in << ".body.end";
    in.seekg(5, std::ios::beg);
    in.seekp(5, std::ios::beg);

    std::stringstream out;

    ByteAssembler::AssemblyInfo info {
        System::Context.InputFiles().at(0),
        0x00
    };

    namespace op = Instructions::OpCodes;
    namespace reg = ModeFlags::RegisterModeFlags;

    SECTION("Instructions::Nop")
    {
        in << "nop";
        Instructions::Nop(info, in, out);
        
        char res;
        out.seekg(0, std::ios::beg);
        out.seekp(0, std::ios::beg);
        out.read(&res, sizeof(res));
        
        CHECK(res == op::nop);
    }

    SECTION("Instructions::StoreConstant")
    {
        const unsigned char expected[] {
            op::stb, 0x0F, 
            op::stb, 0x00, 
            op::stf, 0x3F, 0x80, 0x00, 0x00,
            op::sti, 0x0F, 0x00, 0x00, 0x00
        };
        char res[14] {};
        in << "stc %b 15 stc %b unknown stc 15.0 stc 15";
        for (int i = 0; i < 5; i++)
            Instructions::StoreConstant(info, in, out);

        out.seekg(0, std::ios::beg);
        out.seekp(0, std::ios::beg);
        out.read(res, sizeof(res));

        for (int i = 0; i < sizeof(res); i++)
            CHECK(expected[i] == res[i]);
    }
}
