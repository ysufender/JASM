#pragma once

#include <fstream>
#include <istream>
#include <ostream>

#include "extensions/serialization.hpp"
#include "bytemode/assembler/assembler.hpp"

namespace Instructions
{
    namespace OpCodes
    {
        ENUMERC nop     = 0x00;

        // stc
        ENUMERC sti     = 0x01;
        ENUMERC stf     = 0x02;
        ENUMERC stb     = 0x03;
        ENUMERC stis    = 0x04;
        ENUMERC stfs    = 0x05;
        ENUMERC stbs    = 0x06;

        // ldc
        ENUMERC ldi     = 0x07;
        ENUMERC ldf     = 0x08;
        ENUMERC ldb     = 0x09;


        // rda
        ENUMERC rdi     = 0x0A;
        ENUMERC rdf     = 0x0B;
        ENUMERC rdb     = 0x0C;
        ENUMERC rdr     = 0x0D;
        
        // mov
        ENUMERC movc    = 0x0E;
        ENUMERC movs    = 0x0F;
        ENUMERC movr    = 0x10;
        ENUMERC movi    = 0x11;
        ENUMERC movf    = 0x12;
        ENUMERC movb    = 0x13;

        // add
        ENUMERC addi    = 0x14; 
        ENUMERC addf    = 0x15; 
        ENUMERC addb    = 0x16; 
    }

    inline void Nop(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        Extensions::Serialization::SerializeInteger(OpCodes::nop, out);
    }

    void StoreConstant(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void LoadConstant(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void ReadAddress(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Move(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Add(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void AddSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
}
