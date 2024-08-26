#pragma once

#include <fstream>
#include <istream>
#include <ostream>
#include <string>

#include "JASMConfig.hpp"
#include "extensions/serialization.hpp"
#include "bytemode/assembler/assembler.hpp"

namespace Instructions
{
    namespace OpCodes
    {
        ENUMERC nop     = 0x00;

        // stc
        ENUMERC stt     = 0x01;
        ENUMERC ste     = 0x02;
        ENUMERC stts    = 0x03;
        ENUMERC stes    = 0x04;

        // ldc
        ENUMERC ldt     = 0x05;
        ENUMERC lde     = 0x06;

        // rda
        ENUMERC rdt     = 0x07;
        ENUMERC rde     = 0x08;
        ENUMERC rdr     = 0x09;
        
        // mov
        ENUMERC movc    = 0x0A;
        ENUMERC movs    = 0x0B;
        ENUMERC movr    = 0x0C;

        // add
        ENUMERC addi    = 0x0D; 
        ENUMERC addf    = 0x0E; 
        ENUMERC addb    = 0x0F; 
        ENUMERC addri   = 0x10;
        ENUMERC addrf   = 0x11;
        ENUMERC addrb   = 0x12;

        // adds
        ENUMERC addsi   = 0x13;
        ENUMERC addsf   = 0x14;
        ENUMERC addsb   = 0x15;

        // hcp - scp - rcp
        ENUMERC hcp     = 0x16;
        ENUMERC scp     = 0x17;

        // inc
        ENUMERC inci    = 0x18;
        ENUMERC incf    = 0x19;
        ENUMERC incb    = 0x1A;
        ENUMERC incri   = 0x1B;
        ENUMERC incrf   = 0x1C;
        ENUMERC incrb   = 0x1D;

        // incs
        ENUMERC incsi   = 0x1E;
        ENUMERC incsf   = 0x1F;
        ENUMERC incsb   = 0x20;

        // dcr
        ENUMERC dcri    = 0x21;
        ENUMERC dcrf    = 0x22;
        ENUMERC dcrb    = 0x23;
        ENUMERC dcrri   = 0x24;
        ENUMERC dcrrf   = 0x25;
        ENUMERC dcrrb   = 0x26;

        // dcrs
        ENUMERC dcrsi   = 0x27;
        ENUMERC dcrsf   = 0x28;
        ENUMERC dcrsb   = 0x29;

        // and
        ENUMERC andst   = 0x2A;
        ENUMERC andse   = 0x2B;
        ENUMERC andr    = 0x2C;

        // or 
        ENUMERC orst    = 0x2D;
        ENUMERC orse    = 0x2E;
        ENUMERC orr     = 0x2F;

        // nor 
        ENUMERC norst   = 0x30;
        ENUMERC norse   = 0x31;
        ENUMERC norr    = 0x32;

        // swp
        ENUMERC swpt    = 0x33;
        ENUMERC swpe    = 0x34;
        ENUMERC swpr    = 0x35;

        // dup
        ENUMERC dupt    = 0x36;
        ENUMERC dupe    = 0x37;

        // raw
        ENUMERC raw     = 0x38; 
        ENUMERC raws    = 0x39;

        // 
    }
    
    std::string Nop(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string StoreConstant(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string LoadConstant(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string ReadAddress(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string Move(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string Add(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string AddSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string MemCopy(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string HeapCopy(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string StackCopy(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string Increment(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string IncrementSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string Decrement(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string DecrementSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string And(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string Or(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string Nor(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string Swap(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string Duplicate(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string RawData(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    std::string RomData(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
}
