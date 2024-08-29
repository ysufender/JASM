#pragma once

#include <string>

#include "JASMConfig.hpp"

#define Is8Bit(reg) (reg >= Enumc(ModeFlags::RegisterModeFlags::al) && reg <= Enumc(ModeFlags::RegisterModeFlags::flg))

namespace ModeFlags
{
    inline constexpr uchar_t NoMode = 0x00;

    enum class NumericModeFlags : uchar_t
    {
        Int,
        Float,
        Byte,
        UInt,
        UByte,
    };

    enum class MemoryModeFlags : uchar_t
    {
        Stack,
        Heap,
    };

    enum class RegisterModeFlags : uchar_t
    {
        /* 32-bit */ eax, ebx, ecx, edx, esi, edi,

        /* 8-bit */ al, bl, cl, dl,

        /* 8-bit system */ flg,

        /* 32-bit system */ pc, sp,
    };

    enum class CompareModeFlags : uchar_t
    {
        les, gre, equ, leq, geq, neq,
    };

    uchar_t GetModeFlag(const std::string& identifier, uchar_t start, uchar_t end, bool throws = false);
}
