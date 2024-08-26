#pragma once

#include <string>

#include "JASMConfig.hpp"

namespace ModeFlags
{
    ENUMERC NoMode = 0x00;

    namespace NumericModeFlags 
    {
        ENUMERC Int         = 0x01;
        ENUMERC Float       = 0x02;
        ENUMERC Byte        = 0x03;
        ENUMERC UInt        = 0x04;
        ENUMERC UByte       = 0x05;
    }

    namespace MemoryModeFlags
    {
        ENUMERC Stack       = 0x06;
        ENUMERC Heap        = 0x07;
        // 01100000
        // 00000111
    }

    namespace RegisterModeFlags
    {
        // 32-bit
        ENUMERC eax         = 0x08;
        ENUMERC ebx         = 0x09;
        ENUMERC ecx         = 0x0A;
        ENUMERC edx         = 0x0B;
        ENUMERC esi         = 0x0C;
        ENUMERC edi         = 0x0D;

        // 8-bit
        ENUMERC al          = 0x0E;
        ENUMERC bl          = 0x0F;
        ENUMERC cl          = 0x10;
        ENUMERC dl          = 0x11;

        // 8-bit system
        ENUMERC flg         = 0x12;

        // 32-bit system
        ENUMERC pc          = 0x13;
        ENUMERC sp          = 0x14;
    }

    char GetModeFlag(const std::string& identifier, char start, char end, bool throws = false);
}
