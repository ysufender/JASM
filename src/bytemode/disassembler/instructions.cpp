#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <istream>
#include <iostream>

#include "JASMConfig.hpp"
#include "bytemode/assembler/modeflags.hpp"
#include "extensions/serialization.hpp"

using Reg = ModeFlags::RegisterModeFlags;
using Numo = ModeFlags::NumericModeFlags;
using Memo = ModeFlags::MemoryModeFlags;
using Comp = ModeFlags::CompareModeFlags;

using namespace Extensions::Serialization;

std::unordered_map<uchar_t, std::string_view> modeStr {
    {Enumc(Reg::eax), "&eax"},
    {Enumc(Reg::ebx), "&ebx"},
    {Enumc(Reg::ecx), "&ecx"},
    {Enumc(Reg::edx), "&edx"},
    {Enumc(Reg::esi), "&esi"},
    {Enumc(Reg::edi), "&edi"},

    {Enumc(Reg::al), "&al"},
    {Enumc(Reg::bl), "&bl"},
    {Enumc(Reg::cl), "&cl"},
    {Enumc(Reg::dl), "&dl"},

    {Enumc(Reg::flg), "&flg"},

    {Enumc(Reg::pc), "&pc"},
    {Enumc(Reg::sp), "&sp"},
    {Enumc(Reg::bp), "&bp"},

    {Enumc(Comp::les), "%les"},
    {Enumc(Comp::gre), "%gre"},
    {Enumc(Comp::equ), "%equ"},
    {Enumc(Comp::leq), "%leq"},
    {Enumc(Comp::geq), "%geq"},
    {Enumc(Comp::neq), "%neq"},
};

namespace DisassemblerInstructions
{
    //
    // Implementation
    //
    //
    void Nop(std::istream& in, std::ostream& out) {
        out << "\nnop";
    }

    void Store32(std::istream& in, std::ostream& out)
    {
        systembit_t val;
        DeserializeInteger(val, in);
        out << "\nstc %i/%ui/%f " << val;
    }

    void Store8(std::istream& in, std::ostream& out)
    {
        char val;
        in.read(&val, 1);
        out << "\nstc %b/%ub " << static_cast<uchar_t>(val);
    }

    void Store32Symbol(std::istream& in, std::ostream& out)
    {
        systembit_t val;
        DeserializeInteger(val, in);
        out << "\nstc %i/%ui/%f rom(" << val << ")";
    }

    void Store8Symbol(std::istream& in, std::ostream& out)
    {
        systembit_t val;
        DeserializeInteger(val, in);
        out << "\nstc %b/%ub rom(" << val << ")";
    }

    void Load32(std::istream& in, std::ostream& out)
    {
        out << "\nldc %i/%ui/%f";
    }

    void Load8(std::istream& in, std::ostream& out)
    {
        out << "\nldc %b/%ub";
    }

    void Read32Address(std::istream& in, std::ostream& out)
    {
        out << "\nrda %i/%ui/%f";
    }

    void Read8Address(std::istream& in, std::ostream& out)
    {
        out << "\nrda %b/%ub";
    }

    void ReadRegister(std::istream& in, std::ostream& out)
    {
        char regMode;
        in.read(&regMode, 1);
        out << "\nrda " << modeStr.at(static_cast<uchar_t>(regMode));
    }

    void MoveConstToReg(std::istream& in, std::ostream& out)
    {
        char regMode;
        in.read(&regMode, 1);

        if (Is8Bit(regMode))
        {
            systembit_t val;
            DeserializeInteger(val, in);
            out << "\nmov " << val << modeStr.at(static_cast<uchar_t>(regMode));
        }
        else
        {
            uchar_t val;
            DeserializeInteger(val, in);
            out << "\nmov " << val << modeStr.at(static_cast<uchar_t>(regMode));
        }
    }

    void MoveStackToReg(std::istream& in, std::ostream& out)
    {
        char regMode;
        in.read(&regMode, 1);
        out << "\nmov " << modeStr.at(static_cast<uchar_t>(regMode));
    }

    void MoveRegToReg(std::istream& in, std::ostream& out)
    {
        char lRegMode, rRegMode;
        in.read(&lRegMode, 1);
        in.read(&rRegMode, 1);
        out << "\nmov "
            << modeStr.at(static_cast<uchar_t>(lRegMode))
            << modeStr.at(static_cast<uchar_t>(rRegMode));
    }

    void Add32(std::istream& in, std::ostream& out)
    {
        out << "\nadd %i/%ui";
    }

    void AddFloat(std::istream& in, std::ostream& out)
    {
        out << "\nadd %f";
    }

    void Add8(std::istream& in, std::ostream& out)
    {
        out << "\nadd %b/%ub";
    }

    void AddReg32(std::istream& in, std::ostream& out)
    {
        char lRegMode, rRegMode;
        in.read(&lRegMode, 1);
        in.read(&rRegMode, 1);

        out << "\nadd %i/%ui "
            << modeStr.at(static_cast<uchar_t>(lRegMode))
            << modeStr.at(static_cast<uchar_t>(rRegMode));
    }

    void AddRegFloat(std::istream& in, std::ostream& out)
    {
        char lRegMode, rRegMode;
        in.read(&lRegMode, 1);
        in.read(&rRegMode, 1);

        out << "\nadd %f "
            << modeStr.at(static_cast<uchar_t>(lRegMode))
            << modeStr.at(static_cast<uchar_t>(rRegMode));
    }

    void AddReg8(std::istream& in, std::ostream& out)
    {
        char lRegMode, rRegMode;
        in.read(&lRegMode, 1);
        in.read(&rRegMode, 1);

        out << "\nadd %b/%ub "
            << modeStr.at(static_cast<uchar_t>(lRegMode))
            << modeStr.at(static_cast<uchar_t>(rRegMode));
    }

    void AddSafe32(std::istream& in, std::ostream& out)
    {
        out << "\nadds %i/%ui";
    }

    void AddSafeFloat(std::istream& in, std::ostream& out)
    {
        out << "\nadds %f";
    }

    void AddSafe8(std::istream& in, std::ostream& out)
    {
        out << "\nadds %b/%ub";
    }

    void Sub32(std::istream& in, std::ostream& out)
    {
        out << "\nsub %i/%ui";
    }

    void SubFloat(std::istream& in, std::ostream& out)
    {
        out << "\nsub %f";
    }

    void Sub8(std::istream& in, std::ostream& out)
    {
        out << "\nsub %b/%ub";
    }

    void SubReg32(std::istream& in, std::ostream& out)
    {
        char lRegMode, rRegMode;
        in.read(&lRegMode, 1);
        in.read(&rRegMode, 1);

        out << "\nadd %i/%ui "
            << modeStr.at(static_cast<uchar_t>(lRegMode))
            << modeStr.at(static_cast<uchar_t>(rRegMode));
    }

    void SubRegFloat(std::istream& in, std::ostream& out)
    {
        char lRegMode, rRegMode;
        in.read(&lRegMode, 1);
        in.read(&rRegMode, 1);

        out << "\nadd %f "
            << modeStr.at(static_cast<uchar_t>(lRegMode))
            << modeStr.at(static_cast<uchar_t>(rRegMode));
    }

    void SubReg8(std::istream& in, std::ostream& out)
    {
        char lRegMode, rRegMode;
        in.read(&lRegMode, 1);
        in.read(&rRegMode, 1);

        out << "\nadd %b/%ub "
            << modeStr.at(static_cast<uchar_t>(lRegMode))
            << modeStr.at(static_cast<uchar_t>(rRegMode));
    }

    void SubSafe32(std::istream& in, std::ostream& out)
    {
        out << "\nsubs %i/%ui";
    }

    void SubSafeFloat(std::istream& in, std::ostream& out)
    {
        out << "\nsubs %f";
    }

    void SubSafe8(std::istream& in, std::ostream& out)
    {
        out << "\nsubs %b/%ub";
    }

    void MemCopy(std::istream& in, std::ostream& out)
    {
        in.seekg(1, std::ios::cur);
        out << "\nmcp";
    }

    void Increment32(std::istream& in, std::ostream& out)
    {
        systembit_t val;
        DeserializeInteger(val, in);
        out << "\ninc %i/%ui " << val;
    }

    void IncrementFloat(std::istream& in, std::ostream& out)
    {
        float val;
        DeserializeFloat(val, in);
        out << "\ninc %f " << val;
    }

    void Increment8(std::istream& in, std::ostream& out)
    {
        uchar_t val;
        DeserializeInteger(val, in);
        out << "\ninc %b/%ub " << val;
    }

    void IncrementReg32(std::istream& in, std::ostream& out)
    {
        uchar_t regMode;
        systembit_t val;

        DeserializeInteger(regMode, in);
        DeserializeInteger(val, in);
        out << "\ninc %i/%ui " << modeStr.at(regMode) << " " << val;
    }

    void IncrementRegFloat(std::istream& in, std::ostream& out)
    {
        uchar_t regMode;
        float val;

        DeserializeInteger(regMode, in);
        DeserializeFloat(val, in);
        out << "\ninc %f " << modeStr.at(regMode) << " " << val;
    }

    void IncrementReg8(std::istream& in, std::ostream& out)
    {
        uchar_t regMode;
        uchar_t val;

        DeserializeInteger(regMode, in);
        DeserializeInteger(val, in);
        out << "\ninc %b/%ub " << modeStr.at(regMode) << " " << val;
    }

    void IncrementSafe32(std::istream& in, std::ostream& out)
    {
        systembit_t val;
        DeserializeInteger(val, in);
        out << "\nincs %i/%ui " << val;
    }

    void IncrementSafeFloat(std::istream& in, std::ostream& out)
    {
        float val;
        DeserializeFloat(val, in);
        out << "\nincs %f " << val;
    }

    void IncrementSafe8(std::istream& in, std::ostream& out)
    {
        uchar_t val;
        DeserializeInteger(val, in);
        out << "\nincs %b/%ub " << val;
    }

    void Decrement32(std::istream& in, std::ostream& out)
    {
        systembit_t val;
        DeserializeInteger(val, in);
        out << "\ndcr %i/%ui " << val;
    }

    void Decrement8(std::istream& in, std::ostream& out)
    {
        uchar_t val;
        DeserializeInteger(val, in);
        out << "\ndcr %b/%ub " << val;
    }

    void DecrementFloat(std::istream& in, std::ostream& out)
    {
        float val;
        DeserializeFloat(val, in);
        out << "\ndcr %f " << val;
    }

    void DecrementReg32(std::istream& in, std::ostream& out)
    {
        uchar_t regMode;
        systembit_t val;

        DeserializeInteger(regMode, in);
        DeserializeInteger(val, in);
        out << "\ninc %i/%ui " << modeStr.at(regMode) << " " << val;
    }

    void DecrementReg8(std::istream& in, std::ostream& out)
    {
        uchar_t regMode;
        uchar_t val;

        DeserializeInteger(regMode, in);
        DeserializeInteger(val, in);
        out << "\ninc %b/%ub " << modeStr.at(regMode) << " " << val;
    }

    void DecrementRegFloat(std::istream& in, std::ostream& out)
    {
        uchar_t regMode;
        float val;

        DeserializeInteger(regMode, in);
        DeserializeFloat(val, in);
        out << "\ninc %f " << modeStr.at(regMode) << " " << val;
    }

    void DecrementSafe32(std::istream& in, std::ostream& out)
    {
        systembit_t val;
        DeserializeInteger(val, in);
        out << "\nincs %i/%ui " << val;
    }

    void DecrementSafe8(std::istream& in, std::ostream& out)
    {
        uchar_t val;
        DeserializeInteger(val, in);
        out << "\nincs %b/%ub " << val;
    }

    void DecrementSafeFloat(std::istream& in, std::ostream& out)
    {
        float val;
        DeserializeFloat(val, in);
        out << "\nincs %f " << val;
    }

    void And32(std::istream& in, std::ostream& out)
    {
        out << "\nand %i/%ui/%f";
    }

    void And8(std::istream& in, std::ostream& out)
    {
        out << "\nand %b/%ub";
    }

    void AndReg(std::istream& in, std::ostream& out)
    {
        uchar_t lRegMode, rRegMode;
        DeserializeInteger(lRegMode, in);
        DeserializeInteger(rRegMode, in);

        out << "\nand "
            << modeStr.at(lRegMode)
            << modeStr.at(rRegMode);
    }

    void Or32(std::istream& in, std::ostream& out)
    {
        out << "\nand %i/%ui/%f";
    }

    void Or8(std::istream& in, std::ostream& out)
    {
        out << "\nand %b/%ub";
    }

    void OrReg(std::istream& in, std::ostream& out)
    {
        uchar_t lRegMode, rRegMode;
        DeserializeInteger(lRegMode, in);
        DeserializeInteger(rRegMode, in);

        out << "\nor "
            << modeStr.at(lRegMode)
            << modeStr.at(rRegMode);
    }

    void Nor32(std::istream& in, std::ostream& out)
    {
        out << "\nnor %i/%ui/%f";
    }

    void Nor8(std::istream& in, std::ostream& out)
    {
        out << "\nnor %b/%ub";
    }

    void NorReg(std::istream& in, std::ostream& out)
    {
        uchar_t lRegMode, rRegMode;
        DeserializeInteger(lRegMode, in);
        DeserializeInteger(rRegMode, in);

        out << "\nnor "
            << modeStr.at(lRegMode)
            << modeStr.at(rRegMode);
    }

    void Swap32(std::istream& in, std::ostream& out)
    {
    }

    void Swap8(std::istream& in, std::ostream& out)
    {
    }

    void SwapReg(std::istream& in, std::ostream& out)
    {
    }

    void Duplicate32(std::istream& in, std::ostream& out)
    {
    }

    void Duplicate8(std::istream& in, std::ostream& out)
    {
    }

    void RawData(std::istream& in, std::ostream& out)
    {
        systembit_t size;
        DeserializeInteger(size, in);

        out << "\nraw ";
        for (; size > 0; size--)
            out << "0x" << std::hex << in.get() << ' '; 
        out << ';';
    }

    void RawSymbol(std::istream& in, std::ostream& out)
    {
    }

//  This instruction can't be detected since it leaves no trace
//  void RomData(std::istream& in, std::ostream& out)
//  {
//
//  }

    void Invert32(std::istream& in, std::ostream& out)
    {

    }
    void Invert8(std::istream& in, std::ostream& out)
    {

    }
    void InvertReg(std::istream& in, std::ostream& out)
    {

    }

    void InvertSafe32(std::istream& in, std::ostream& out)
    {

    }
    void InvertSafe8(std::istream& in, std::ostream& out)
    {

    }
    
    void CompareStack(std::istream& in, std::ostream& out)
    {

    }
    void CompareReg(std::istream& in, std::ostream& out)
    {

    }

    void Pop32(std::istream& in, std::ostream& out)
    {

    }
    void Pop8(std::istream& in, std::ostream& out)
    {

    }

    void JumpImm(std::istream& in, std::ostream& out)
    {

    }
    void JumpReg(std::istream& in, std::ostream& out)
    {

    }

    void SwapRange(std::istream& in, std::ostream& out)
    {

    }

    void DuplicateRange(std::istream& in, std::ostream& out)
    {

    }

    void Repeat(std::istream& in, std::ostream& out)
    {

    }

    void Allocate(std::istream& in, std::ostream& out)
    {

    }

    void PowerImm32(std::istream& in, std::ostream& out)
    {

    }
    void PowerImm8(std::istream& in, std::ostream& out)
    {

    }
    void PowerImmFloat(std::istream& in, std::ostream& out)
    {

    }

    void PowerStack32(std::istream& in, std::ostream& out)
    {

    }
    void PowerStack8(std::istream& in, std::ostream& out)
    {

    }
    void PowerStackFloat(std::istream& in, std::ostream& out)
    {

    }

    void PowerReg32(std::istream& in, std::ostream& out)
    {

    }
    void PowerReg8(std::istream& in, std::ostream& out)
    {

    }
    void PowerRegFloat(std::istream& in, std::ostream& out)
    {

    }

    void SquareImm32(std::istream& in, std::ostream& out)
    {

    }
    void SquareImm8(std::istream& in, std::ostream& out)
    {

    }
    void SquareImmFloat(std::istream& in, std::ostream& out)
    {

    }

    void SquareStack32(std::istream& in, std::ostream& out)
    {

    }
    void SquareStack8(std::istream& in, std::ostream& out)
    {

    }
    void SquareStackFloat(std::istream& in, std::ostream& out)
    {

    }

    void SquareReg32(std::istream& in, std::ostream& out)
    {

    }
    void SquareReg8(std::istream& in, std::ostream& out)
    {

    }
    void SquareRegFloat(std::istream& in, std::ostream& out)
    {

    }

    void ConditionalImm(std::istream& in, std::ostream& out)
    {

    }
    void ConditionalReg(std::istream& in, std::ostream& out)
    {

    }

    void CallImm(std::istream& in, std::ostream& out)
    {

    }
    void CallReg(std::istream& in, std::ostream& out)
    {

    }

    void Multiply32(std::istream& in, std::ostream& out)
    {

    }
    void Multiply8(std::istream& in, std::ostream& out)
    {

    }
    void MultiplyFloat(std::istream& in, std::ostream& out)
    {

    }

    void MultiplyReg32(std::istream& in, std::ostream& out)
    {

    }
    void MultiplyReg8(std::istream& in, std::ostream& out)
    {

    }
    void MultiplyRegFloat(std::istream& in, std::ostream& out)
    {

    }

    void MultiplySafe32(std::istream& in, std::ostream& out)
    {

    }
    void MultiplySafe8(std::istream& in, std::ostream& out)
    {

    }
    void MultiplySafeFloat(std::istream& in, std::ostream& out)
    {

    }

    void Divide32(std::istream& in, std::ostream& out)
    {

    }
    void Divide8(std::istream& in, std::ostream& out)
    {

    }
    void DivideFloat(std::istream& in, std::ostream& out)
    {

    }

    void DivideReg32(std::istream& in, std::ostream& out)
    {

    }
    void DivideReg8(std::istream& in, std::ostream& out)
    {

    }
    void DivideRegFloat(std::istream& in, std::ostream& out)
    {

    }

    void DivideSafe32(std::istream& in, std::ostream& out)
    {

    }
    void DivideSafe8(std::istream& in, std::ostream& out)
    {

    }
    void DivideSafeFloat(std::istream& in, std::ostream& out)
    {

    }

    void Return(std::istream& in, std::ostream& out)
    {

    }

    void Deallocate(std::istream& in, std::ostream& out)
    {

    }

    void IncrementLocal32(std::istream& in, std::ostream& out)
    {

    }
    void IncrementLocal8(std::istream& in, std::ostream& out)
    {

    }
    void IncrementLocalFloat(std::istream& in, std::ostream& out)
    {

    }

    void ReadLocal32(std::istream& in, std::ostream& out)
    {

    }

    void ReadLocal8(std::istream& in, std::ostream& out)
    {

    }

    void CompareJump(std::istream& in, std::ostream& out)
    {

    }

    void CompareLocal(std::istream& in, std::ostream& out)
    {

    }

    void SetFlag(std::istream& in, std::ostream& out)
    {

    }

    void SysCall(std::istream& in, std::ostream& out)
    {
        systembit_t size;
        DeserializeInteger(size, in);

        out << "\nsys \"";
        for (; size > 0; size--)
            out << char(in.get());
        out << "\"";
    }

    void Xor32(std::istream& in, std::ostream& out)
    {

    }

    void Xor8(std::istream& in, std::ostream& out)
    {

    }

    void XorReg(std::istream& in, std::ostream& out)
    {

    }
}
