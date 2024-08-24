#pragma once

#include <fstream>
#include <istream>
#include <ostream>
#include <string>

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

        // add
        ENUMERC addi    = 0x11; 
        ENUMERC addf    = 0x12; 
        ENUMERC addb    = 0x13; 

        // addr
        ENUMERC addri   = 0x14;
        ENUMERC addrf   = 0x15;
        ENUMERC addrb   = 0x16;

        // addr
        ENUMERC addsi   = 0x17;
        ENUMERC addsf   = 0x18;
        ENUMERC addsb   = 0x19;

        // hcp - scp - rcp
        ENUMERC hcp     = 0x1A;
        ENUMERC scp     = 0x1B;
        //ENUMERC rcp     = 0x1C;

        // inc
        ENUMERC inci    = 0x1D;
        ENUMERC incf    = 0x1E;
        ENUMERC incb    = 0x1F;
        
        // incr
        ENUMERC incr    = 0x20;

        // incs
        ENUMERC incsi    = 0x21;
        ENUMERC incsf    = 0x22;
        ENUMERC incsb    = 0x23;
    }

    std::string Nop(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string StoreConstant(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string LoadConstant(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string ReadAddress(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string Move(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string AddStack(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string AddRegister(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string AddSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string HeapCopy(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string StackCopy(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    //std::string RomCopy(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string IncrementStack(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string IncrementRegister(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string IncrementSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
}
