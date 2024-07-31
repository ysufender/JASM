#include <unordered_map>

#include "assembler/modeflags.hpp"
#include "extensions/system.hpp"

namespace ModeFlags
{
    static const std::unordered_map<std::string, char> nuModeMap {
        {"%i", NumericModeFlags::Int},
        {"%f", NumericModeFlags::Float},
        {"%b", NumericModeFlags::Byte},
        {"%ui", NumericModeFlags::UInt},
        {"%ub", NumericModeFlags::UByte},
    };

    static const std::unordered_map<std::string, char> regModeMap {
        {"&eax", RegisterModeFlags::eax},
        {"&ebx", RegisterModeFlags::ebx},
        {"&ecx", RegisterModeFlags::ecx},
        {"&edx", RegisterModeFlags::edx},

        {"&al", RegisterModeFlags::al},
        {"&bl", RegisterModeFlags::bl},
        {"&cl", RegisterModeFlags::cl},
        {"&dl", RegisterModeFlags::dl},
    };

    void _ErrCall(const std::string& identifier)
    {
        LOGE(System::LogLevel::High, "Couldn't find mode flag for '", identifier, "'");
    };

    char GetModeFlag(const std::string& identifier, bool throws)
    {
        if (!nuModeMap.contains(identifier) && throws)
            _ErrCall(identifier);

        return nuModeMap.contains(identifier) ? nuModeMap.at(identifier) : NoMode;
    }

    char GetRegisterModeFlag(const std::string& identifier, bool throws)
    {
        return identifier.starts_with('&') ? regModeMap.at(identifier) : ModeFlags::NoMode;

        if (!regModeMap.contains(identifier) && throws)
            _ErrCall(identifier);

        return regModeMap.contains(identifier) ? regModeMap.at(identifier) : NoMode;
    }
}

//
// Tests
//
#include "JASMConfig.hpp"
#include "test/test.hpp"

#ifdef TEST_MODE
    TEST_CASE("ModeFlags::Register")
    {
        for (const auto& [reg, regFlag] : ModeFlags::regModeMap)
            CHECK_FALSE(ModeFlags::GetRegisterModeFlag(reg) == regFlag);

        CHECK_THROWS_MESSAGE(ModeFlags::GetRegisterModeFlag("unknown", true), "Couldn't find mode flag for 'unknown'");
        CHECK(ModeFlags::GetRegisterModeFlag("unknown") == ModeFlags::NoMode);
    }

    TEST_CASE("ModeFlags::Mode")
    {
        for (const auto& [nu, flag] : ModeFlags::nuModeMap)
            CHECK(ModeFlags::GetModeFlag(nu) == flag);

        CHECK_THROWS_MESSAGE(ModeFlags::GetModeFlag("unknown", true), "Couldn't find mode flag for 'unknown'");
        CHECK(ModeFlags::GetModeFlag("unknown") == ModeFlags::NoMode);
    }
#endif
