#pragma once

#include <istream>
#include <ostream>

#include <cmath>

#include "JASMConfig.hpp"
#include "extensions/serialization.hpp"
#include "bytemode/assembler/assembler.hpp"
#include "bytemode/assembler/instructions.hpp"

namespace DisassemblerInstructions
{
    void Nop(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void StoreConstant(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void LoadConstant(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void ReadAddress(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Move(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Add(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void AddSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void MemCopy(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Increment(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void IncrementSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Decrement(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void DecrementSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void And(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Or(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Nor(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Swap(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Duplicate(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void RawData(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void RomData(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Invert(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void InvertSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Compare(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Pop(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Jump(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void SwapRange(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void DuplicateRange(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void DuplicateRange(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Repeat(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Allocate(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Power(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void SquareRoot(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Conditional(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Call(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Multiply(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void MultiplySafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Divide(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void DivideSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Return(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Deallocate(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void Sub(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void SubSafe(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void IncrementLocal(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void ReadLocal(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void ConditionalJump(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
    void CompareLocal(ByteAssembler::AssemblyInfo& info, std::istream& in, std::ostream& out);
}
