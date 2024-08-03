#pragma once

#include <string>

namespace ModeFlags
{
    inline constexpr char NoMode = 0x00;

    namespace NumericModeFlags 
    {
        inline constexpr char Int       = 0x01;
        inline constexpr char Float     = 0x02;
        inline constexpr char Byte      = 0x03;
        inline constexpr char UInt      = 0x04;
        inline constexpr char UByte     = 0x05;
    }

    namespace RegisterModeFlags
    {
        // 32-bit
        inline constexpr char eax       = 0x08;
        inline constexpr char ebx       = 0x09;
        inline constexpr char ecx       = 0x0A;
        inline constexpr char edx       = 0x0B;

        // 8-bit
        inline constexpr char al        = 0x0C;
        inline constexpr char bl        = 0x0D;
        inline constexpr char cl        = 0x0E;
        inline constexpr char dl        = 0x0F;
    }

    char GetModeFlag(const std::string& identifier, bool throws = false); 
    char GetRegisterModeFlag(const std::string& identifier, bool throws = false);
}
