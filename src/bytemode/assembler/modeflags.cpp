#include <string>
#include <unordered_map>

#include "JASMConfig.hpp"

#include "bytemode/assembler/modeflags.hpp"
#include "system.hpp"

namespace ModeFlags
{
    std::unordered_map<std::string, uchar_t> modeMap {
        {"%i", Enumc(NumericModeFlags::Int)},
        {"%f", Enumc(NumericModeFlags::Float)},
        {"%b", Enumc(NumericModeFlags::Byte)},
        {"%ui", Enumc(NumericModeFlags::UInt)},
        {"%ub", Enumc(NumericModeFlags::UByte)},

        {"%s", Enumc(MemoryModeFlags::Stack)},
        {"%h", Enumc(MemoryModeFlags::Heap)},

        {"&eax", Enumc(RegisterModeFlags::eax)},
        {"&ebx", Enumc(RegisterModeFlags::ebx)},
        {"&ecx", Enumc(RegisterModeFlags::ecx)},
        {"&edx", Enumc(RegisterModeFlags::edx)},
        {"&esi", Enumc(RegisterModeFlags::esi)},
        {"&edi", Enumc(RegisterModeFlags::edi)},

        {"&al", Enumc(RegisterModeFlags::al)},
        {"&bl", Enumc(RegisterModeFlags::bl)},
        {"&cl", Enumc(RegisterModeFlags::cl)},
        {"&dl", Enumc(RegisterModeFlags::dl)},

        {"&flg", Enumc(RegisterModeFlags::flg)},

        {"&pc", Enumc(RegisterModeFlags::pc)},
        {"&sp", Enumc(RegisterModeFlags::sp)},

        {"%les", Enumc(CompareModeFlags::les)},
        {"%gre", Enumc(CompareModeFlags::gre)},
        {"%equ", Enumc(CompareModeFlags::equ)},
        {"%leq", Enumc(CompareModeFlags::leq)},
        {"%geq", Enumc(CompareModeFlags::geq)},
        {"%neq", Enumc(CompareModeFlags::neq)},
    };

    uchar_t GetModeFlag(const std::string &identifier, uchar_t start, uchar_t end, bool throws)
    {
            if (!modeMap.contains(identifier) && throws)
                LOGE(System::LogLevel::High, "Couldn't fin mode flag for '", identifier, "'.");
            else if (!modeMap.contains(identifier))
                return NoMode;

            uchar_t mode { modeMap.at(identifier) };
            bool isInRange { start <= mode && mode <= end };

            if (throws && !isInRange)
                LOGE(System::LogLevel::High, "'", identifier, "' is not a suitable mode flag for this scenario.");
            else if (isInRange)
                return mode;

            return NoMode;
    }
}
