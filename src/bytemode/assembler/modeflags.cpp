#include <string>
#include <unordered_map>

#include "JASMConfig.hpp"

#include "bytemode/assembler/modeflags.hpp"
#include "system.hpp"

namespace ModeFlags
{
    TESTCONST std::unordered_map<std::string, char> modeMap {
        {"%i", NumericModeFlags::Int},
        {"%f", NumericModeFlags::Float},
        {"%b", NumericModeFlags::Byte},
        {"%ui", NumericModeFlags::UInt},
        {"%ub", NumericModeFlags::UByte},

        {"%s", MemoryModeFlags::Stack},
        {"%h", MemoryModeFlags::Heap},

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

    char GetModeFlag(const std::string &identifier, char start, char end, bool throws)
    {
            if (!modeMap.contains(identifier) && throws)
                _ErrCall(identifier);
            else if (!modeMap.contains(identifier))
                return NoMode;

            char mode { modeMap.at(identifier) };
            bool isInRange { start <= mode && mode <= end };

            if (throws && !isInRange)
                _ErrCall(identifier);
            else if (isInRange)
                return mode;

            return NoMode;
    }
}
