#pragma once

#include <fstream>
#include <istream>

#include "extensions/serialization.hpp"
#include "assembler/assembler.hpp"

namespace Instructions
{
    namespace OpCodes
    {
        inline constexpr char nop   = 0x00;

        // stc
        inline constexpr char sti   = 0x01;
        inline constexpr char stf   = 0x02;
        inline constexpr char stb   = 0x03;

        // ldc
        inline constexpr char ldi   = 0x04;
        inline constexpr char ldf   = 0x05;
        inline constexpr char ldb   = 0x06;


        // rda
        inline constexpr char rdi   = 0x07;
        inline constexpr char rdf   = 0x08;
        inline constexpr char rdb   = 0x09;
        inline constexpr char rdr   = 0x0A;
        
        // mov
        inline constexpr char movi  = 0x0B;
        inline constexpr char movf  = 0x0C;
        inline constexpr char movb  = 0x0D;
        inline constexpr char movr  = 0x0E;
    }

    inline void Nop(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        Extensions::Serialization::SerializeInteger(OpCodes::nop, out);
    }

    void StoreConstant(AssemblyInfo& info, std::istream& in, std::ostream& out);
    void LoadConstant(AssemblyInfo& info, std::istream& in, std::ostream& out);
    void ReadAddress(AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Move(AssemblyInfo& info, std::istream& in, std::ostream& out);
}