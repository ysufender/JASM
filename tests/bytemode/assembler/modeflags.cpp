#include <string>
#include <unordered_map>

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include "bytemode/assembler/modeflags.hpp"

namespace ModeFlags
{
    extern const std::unordered_map<std::string, char> regModeMap; 
    extern const std::unordered_map<std::string, char> nuModeMap; 
}

TEST_CASE("ModeFlags Tests")
{
    SECTION("ModeFlags::Register")
    {
        for (const auto& [reg, flag] : ModeFlags::regModeMap)
            CHECK(ModeFlags::GetRegisterModeFlag(reg) == flag); 

        CHECK_THROWS(ModeFlags::GetRegisterModeFlag("unknown", true));

        CHECK_NOTHROW(ModeFlags::GetRegisterModeFlag("unknown"));
        CHECK(ModeFlags::GetRegisterModeFlag("unknown") == ModeFlags::NoMode);
    }

    SECTION("ModeFlags::Number")
    {
        for (const auto&  [num, flag] : ModeFlags::nuModeMap)
            CHECK(ModeFlags::GetModeFlag(num) == flag);

        CHECK_THROWS(ModeFlags::GetModeFlag("unknown", true));

        CHECK_NOTHROW(ModeFlags::GetModeFlag("unknown"));
        CHECK(ModeFlags::GetModeFlag("unknown") == ModeFlags::NoMode);
    }
}
