#pragma once

#include <istream>

namespace DisassemblerInstructions
{
    void Nop(std::istream& in, std::ostream& out);

    void Store32(std::istream& in, std::ostream& out);
    void Store8(std::istream& in, std::ostream& out);

    void Store32Symbol(std::istream& in, std::ostream& out);
    void Store8Symbol(std::istream& in, std::ostream& out);

    void Load32(std::istream& in, std::ostream& out);
    void Load8(std::istream& in, std::ostream& out);

    void Read32Address(std::istream& in, std::ostream& out);
    void Read8Address(std::istream& in, std::ostream& out);
    void ReadRegister(std::istream& in, std::ostream& out);

    void MoveStackToReg(std::istream& in, std::ostream& out);
    void MoveRegToReg(std::istream& in, std::ostream& out);
    void MoveConstToReg(std::istream& in, std::ostream& out);

    void Add32(std::istream& in, std::ostream& out);
    void Add8(std::istream& in, std::ostream& out);
    void AddFloat(std::istream& in, std::ostream& out);

    void AddReg32(std::istream& in, std::ostream& out);
    void AddReg8(std::istream& in, std::ostream& out);
    void AddRegFloat(std::istream& in, std::ostream& out);

    void AddSafe32(std::istream& in, std::ostream& out);
    void AddSafe8(std::istream& in, std::ostream& out);
    void AddSafeFloat(std::istream& in, std::ostream& out);

    void Sub32(std::istream& in, std::ostream& out);
    void Sub8(std::istream& in, std::ostream& out);
    void SubFloat(std::istream& in, std::ostream& out);

    void SubReg32(std::istream& in, std::ostream& out);
    void SubReg8(std::istream& in, std::ostream& out);
    void SubRegFloat(std::istream& in, std::ostream& out);

    void SubSafe32(std::istream& in, std::ostream& out);
    void SubSafe8(std::istream& in, std::ostream& out);
    void SubSafeFloat(std::istream& in, std::ostream& out);

    void MemCopy(std::istream& in, std::ostream& out);

    void Increment32(std::istream& in, std::ostream& out);
    void Increment8(std::istream& in, std::ostream& out);
    void IncrementFloat(std::istream& in, std::ostream& out);

    void IncrementReg32(std::istream& in, std::ostream& out);
    void IncrementReg8(std::istream& in, std::ostream& out);
    void IncrementRegFloat(std::istream& in, std::ostream& out);

    void IncrementSafe32(std::istream& in, std::ostream& out);
    void IncrementSafe8(std::istream& in, std::ostream& out);
    void IncrementSafeFloat(std::istream& in, std::ostream& out);

    void Decrement32(std::istream& in, std::ostream& out);
    void Decrement8(std::istream& in, std::ostream& out);
    void DecrementFloat(std::istream& in, std::ostream& out);

    void DecrementReg32(std::istream& in, std::ostream& out);
    void DecrementReg8(std::istream& in, std::ostream& out);
    void DecrementRegFloat(std::istream& in, std::ostream& out);

    void DecrementSafe32(std::istream& in, std::ostream& out);
    void DecrementSafe8(std::istream& in, std::ostream& out);
    void DecrementSafeFloat(std::istream& in, std::ostream& out);

    void And32(std::istream& in, std::ostream& out);
    void And8(std::istream& in, std::ostream& out);
    void AndReg(std::istream& in, std::ostream& out);

    void Or32(std::istream& in, std::ostream& out);
    void Or8(std::istream& in, std::ostream& out);
    void OrReg(std::istream& in, std::ostream& out);

    void Nor32(std::istream& in, std::ostream& out);
    void Nor8(std::istream& in, std::ostream& out);
    void NorReg(std::istream& in, std::ostream& out);

    void Swap32(std::istream& in, std::ostream& out);
    void Swap8(std::istream& in, std::ostream& out);
    void SwapReg(std::istream& in, std::ostream& out);

    void Duplicate32(std::istream& in, std::ostream& out);
    void Duplicate8(std::istream& in, std::ostream& out);

    void RawData(std::istream& in, std::ostream& out);
    void RawSymbol(std::istream& in, std::ostream& out);

//  This instruction can't be detected since it leaves no trace
//  void RomData(std::istream& in, std::ostream& out);

    void Invert32(std::istream& in, std::ostream& out);
    void Invert8(std::istream& in, std::ostream& out);
    void InvertReg(std::istream& in, std::ostream& out);

    void InvertSafe32(std::istream& in, std::ostream& out);
    void InvertSafe8(std::istream& in, std::ostream& out);
    
    void CompareStack(std::istream& in, std::ostream& out);
    void CompareReg(std::istream& in, std::ostream& out);

    void Pop32(std::istream& in, std::ostream& out);
    void Pop8(std::istream& in, std::ostream& out);

    void JumpImm(std::istream& in, std::ostream& out);
    void JumpReg(std::istream& in, std::ostream& out);

    void SwapRange(std::istream& in, std::ostream& out);

    void DuplicateRange(std::istream& in, std::ostream& out);

    void Repeat(std::istream& in, std::ostream& out);

    void Allocate(std::istream& in, std::ostream& out);

    void PowerImm32(std::istream& in, std::ostream& out);
    void PowerImm8(std::istream& in, std::ostream& out);
    void PowerImmFloat(std::istream& in, std::ostream& out);

    void PowerStack32(std::istream& in, std::ostream& out);
    void PowerStack8(std::istream& in, std::ostream& out);
    void PowerStackFloat(std::istream& in, std::ostream& out);

    void PowerReg32(std::istream& in, std::ostream& out);
    void PowerReg8(std::istream& in, std::ostream& out);
    void PowerRegFloat(std::istream& in, std::ostream& out);

    void SquareImm32(std::istream& in, std::ostream& out);
    void SquareImm8(std::istream& in, std::ostream& out);
    void SquareImmFloat(std::istream& in, std::ostream& out);

    void SquareStack32(std::istream& in, std::ostream& out);
    void SquareStack8(std::istream& in, std::ostream& out);
    void SquareStackFloat(std::istream& in, std::ostream& out);

    void SquareReg32(std::istream& in, std::ostream& out);
    void SquareReg8(std::istream& in, std::ostream& out);
    void SquareRegFloat(std::istream& in, std::ostream& out);

    void ConditionalImm(std::istream& in, std::ostream& out);
    void ConditionalReg(std::istream& in, std::ostream& out);

    void CallImm(std::istream& in, std::ostream& out);
    void CallReg(std::istream& in, std::ostream& out);

    void Multiply32(std::istream& in, std::ostream& out);
    void Multiply8(std::istream& in, std::ostream& out);
    void MultiplyFloat(std::istream& in, std::ostream& out);

    void MultiplyReg32(std::istream& in, std::ostream& out);
    void MultiplyReg8(std::istream& in, std::ostream& out);
    void MultiplyRegFloat(std::istream& in, std::ostream& out);

    void MultiplySafe32(std::istream& in, std::ostream& out);
    void MultiplySafe8(std::istream& in, std::ostream& out);
    void MultiplySafeFloat(std::istream& in, std::ostream& out);

    void Divide32(std::istream& in, std::ostream& out);
    void Divide8(std::istream& in, std::ostream& out);
    void DivideFloat(std::istream& in, std::ostream& out);

    void DivideReg32(std::istream& in, std::ostream& out);
    void DivideReg8(std::istream& in, std::ostream& out);
    void DivideRegFloat(std::istream& in, std::ostream& out);

    void DivideSafe32(std::istream& in, std::ostream& out);
    void DivideSafe8(std::istream& in, std::ostream& out);
    void DivideSafeFloat(std::istream& in, std::ostream& out);

    void Return(std::istream& in, std::ostream& out);

    void Deallocate(std::istream& in, std::ostream& out);

    void IncrementLocal32(std::istream& in, std::ostream& out);
    void IncrementLocal8(std::istream& in, std::ostream& out);
    void IncrementLocalFloat(std::istream& in, std::ostream& out);

    void ReadLocal32(std::istream& in, std::ostream& out);
    void ReadLocal8(std::istream& in, std::ostream& out);

    void CompareJump(std::istream& in, std::ostream& out);

    void CompareLocal(std::istream& in, std::ostream& out);

    void SetFlag(std::istream& in, std::ostream& out);

    void SysCall(std::istream& in, std::ostream& out);

    void Xor32(std::istream& in, std::ostream& out);
    void Xor8(std::istream& in, std::ostream& out);
    void XorReg(std::istream& in, std::ostream& out);
}
