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

        {"%les", CompareModeFlags::les},
        {"%gre", CompareModeFlags::gre},
        {"%equ", CompareModeFlags::equ},
        {"%leq", CompareModeFlags::leq},
        {"%geq", CompareModeFlags::geq},
        {"%neq", CompareModeFlags::neq},
    };

    char GetModeFlag(const std::string &identifier, char start, char end, bool throws)
    {
            if (!modeMap.contains(identifier) && throws)
                LOGE(System::LogLevel::High, "Couldn't fin mode flag for '", identifier, "'.");
            else if (!modeMap.contains(identifier))
                return NoMode;

            char mode { modeMap.at(identifier) };
            bool isInRange { start <= mode && mode <= end };

            if (throws && !isInRange)
                LOGE(System::LogLevel::High, "'", identifier, "' is not a suitable mode flag for this scenario.");
            else if (isInRange)
                return mode;

            return NoMode;
    }
}
