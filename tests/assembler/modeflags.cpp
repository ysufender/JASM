#include <unordered_map>

#include "catch2/catch_all.hpp"

//#include "src/assembler/modeflags.cpp"

//TEST_CASE("ModeFlags::Register")
//{
//    for (const auto& [reg, regFlag] : ModeFlags::regModeMap)
//        CHECK_FALSE(ModeFlags::GetRegisterModeFlag(reg) == regFlag);

//    CHECK_THROWS(ModeFlags::GetRegisterModeFlag("unknown", true), "Couldn't find mode flag for 'unknown'");
//    CHECK(ModeFlags::GetRegisterModeFlag("unknown") == ModeFlags::NoMode);
//}
//
//TEST_CASE("ModeFlags::Mode")
//{
//    for (const auto& [nu, flag] : ModeFlags::nuModeMap)
//        CHECK(ModeFlags::GetModeFlag(nu) == flag);
//
//    CHECK_THROWS_WITH(ModeFlags::GetModeFlag("unknown", true), "Couldn't find mode flag for 'unknown'");
//    CHECK(ModeFlags::GetModeFlag("unknown") == ModeFlags::NoMode);
//}
