#pragma once

#include <fstream>
#include <istream>
#include <ostream>

#include "extensions/serialization.hpp"
#include "assembler/assembler.hpp"

namespace Instructions
{
    namespace OpCodes
    {
        ENUMER char nop     = 0x00;

        // stc
        ENUMER char sti     = 0x01;
        ENUMER char stf     = 0x02;
        ENUMER char stb     = 0x03;
        ENUMER char stis    = 0x04;
        ENUMER char stfs    = 0x05;
        ENUMER char stbs    = 0x06;

        // ldc
        ENUMER char ldi     = 0x07;
        ENUMER char ldf     = 0x08;
        ENUMER char ldb     = 0x09;


        // rda
        ENUMER char rdi     = 0x0A;
        ENUMER char rdf     = 0x0B;
        ENUMER char rdb     = 0x0C;
        ENUMER char rdr     = 0x0D;
        
        // mov
        ENUMER char movc    = 0x0E;
        ENUMER char movs    = 0x0F;
        ENUMER char movr    = 0x10;
        ENUMER char movi    = 0x11;
        ENUMER char movf    = 0x12;
        ENUMER char movb    = 0x13;

        // add
        ENUMER char addi    = 0x14; 
        ENUMER char addf    = 0x15; 
        ENUMER char addb    = 0x16; 
    }

    inline void Nop(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        Extensions::Serialization::SerializeInteger(OpCodes::nop, out);
    }

    void StoreConstant(AssemblyInfo& info, std::istream& in, std::ostream& out);
    void LoadConstant(AssemblyInfo& info, std::istream& in, std::ostream& out);
    void ReadAddress(AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Move(AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Add(AssemblyInfo& info, std::istream& in, std::ostream& out);
    void AddSafe(AssemblyInfo& info, std::istream& in, std::ostream& out);
}
