#include <unordered_map>

#include "JASMConfig.hpp"

#include "bytemode/assembler/modeflags.hpp"
#include "system.hpp"

namespace ModeFlags
{
    TESTCONST std::unordered_map<std::string, char> nuModeMap {
        {"%i", NumericModeFlags::Int},
        {"%f", NumericModeFlags::Float},
        {"%b", NumericModeFlags::Byte},
        {"%ui", NumericModeFlags::UInt},
        {"%ub", NumericModeFlags::UByte},
    };

    TESTCONST std::unordered_map<std::string, char> regModeMap {
        {"&eax", RegisterModeFlags::eax},
        {"&ebx", RegisterModeFlags::ebx},
        {"&ecx", RegisterModeFlags::ecx},
        {"&edx", RegisterModeFlags::edx},
        {"&esi", RegisterModeFlags::esi},
        {"&edi", RegisterModeFlags::edi},

        {"&al", RegisterModeFlags::al},
        {"&bl", RegisterModeFlags::bl},
        {"&cl", RegisterModeFlags::cl},
        {"&dl", RegisterModeFlags::dl},

        {"&flg", RegisterModeFlags::flg},

        {"&pc", RegisterModeFlags::pc},
        {"&sp", RegisterModeFlags::sp},
    };

    void _ErrCall(const std::string& identifier)
    {
        LOGE(System::LogLevel::High, "Couldn't find mode flag for '", identifier, "'.");
    };

    char GetModeFlag(const std::string& identifier, bool throws)
    {
        if (!nuModeMap.contains(identifier) && throws)
            _ErrCall(identifier);

        return nuModeMap.contains(identifier) ? nuModeMap.at(identifier) : NoMode;
    }

    char GetRegisterModeFlag(const std::string& identifier, bool throws)
    {
        if (!regModeMap.contains(identifier) && throws)
            _ErrCall(identifier);

        return regModeMap.contains(identifier) ? regModeMap.at(identifier) : NoMode;
    }
}
