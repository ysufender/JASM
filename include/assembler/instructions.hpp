#pragma once

#include <fstream>

#include "assembler/assembler.hpp"

namespace Instructions
{
    namespace OpCodes
    {
        inline constexpr char nop = 0x00;
        inline constexpr char sti = 0x01;
        inline constexpr char stf = 0x02;
        inline constexpr char stb = 0x03;
    }

    void Nop(AssemblyInfo& info, std::ifstream& in, std::ofstream& out);
    void StoreInt(AssemblyInfo& info, std::ifstream& in, std::ofstream& out);
    void StoreFloat(AssemblyInfo& info, std::ifstream& in, std::ofstream& out);
    void StoreByte(AssemblyInfo& info, std::ifstream& in, std::ofstream& out);
    void StoreUInt(AssemblyInfo& info, std::ifstream& in, std::ofstream& out);
    void StoreUByte(AssemblyInfo& info, std::ifstream& in, std::ofstream& out);
}