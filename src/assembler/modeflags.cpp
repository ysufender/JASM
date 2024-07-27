#include <unordered_map>

#include "assembler/modeflags.hpp"


namespace ModeFlags
{
    static const std::unordered_map<std::string, char> operationModeFlagMap {
        {"%i", NumericModeFlags::Int},
        {"%f", NumericModeFlags::Float},
        {"%b", NumericModeFlags::Byte},
        {"%ui", NumericModeFlags::UInt},
        {"%ub", NumericModeFlags::UByte},
    };

    static const std::unordered_map<std::string, char> registerModeMap {
        {"&eax", RegisterModeFlags::eax},
        {"&ebx", RegisterModeFlags::ebx},
        {"&ecx", RegisterModeFlags::ecx},
        {"&edx", RegisterModeFlags::edx},

        {"&al", RegisterModeFlags::al},
        {"&bl", RegisterModeFlags::bl},
        {"&cl", RegisterModeFlags::cl},
        {"&dl", RegisterModeFlags::dl},
    };

    char GetModeFlag(const std::string& identifier)
    {
        return identifier.starts_with('%') ? operationModeFlagMap.at(identifier) : ModeFlags::NoMode;
    }

    char GetRegisterModeFlag(const std::string& identifier)
    {
        return identifier.starts_with('&') ? registerModeMap.at(identifier) : ModeFlags::NoMode;
    }
}