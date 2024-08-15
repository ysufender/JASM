#pragma once

#include <string>

#include "JASMConfig.hpp"

namespace ModeFlags
{
    ENUMER char NoMode = 0x00;

    namespace NumericModeFlags 
    {
        ENUMER char Int         = 0x01;
        ENUMER char Float       = 0x02;
        ENUMER char Byte        = 0x03;
        ENUMER char UInt        = 0x04;
        ENUMER char UByte       = 0x05;
    }

    namespace RegisterModeFlags
    {
        // 32-bit
        ENUMER char eax         = 0x08;
        ENUMER char ebx         = 0x09;
        ENUMER char ecx         = 0x0A;
        ENUMER char edx         = 0x0B;

        // 8-bit
        ENUMER char al          = 0x0C;
        ENUMER char bl          = 0x0D;
        ENUMER char cl          = 0x0E;
        ENUMER char dl          = 0x0F;

        // 8-bit system
        ENUMER char FLG         = 0x10;

        // 32-bit system
        ENUMER char PC          = 0x11;
        ENUMER char SP          = 0x12;
    }

    char GetModeFlag(const std::string& identifier, bool throws = false); 
    char GetRegisterModeFlag(const std::string& identifier, bool throws = false);
}
