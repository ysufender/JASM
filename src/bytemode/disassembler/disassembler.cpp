#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "system.hpp"
#include "JASMConfig.hpp"
#include "assemblycontext.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "bytemode/assembler/assembler.hpp"
#include "bytemode/disassembler/instructions.hpp"
#include "bytemode/disassembler/disassembler.hpp"

#ifdef TOOLCHAIN_MODE
#define CONTEXT this->context
#else
#define CONTEXT System::Context
#endif

namespace ByteAssembler
{
    //
    // Usings
    //
    using namespace Extensions;

    //
    // Assembler Implementation
    //
#ifdef TOOLCHAIN_MODE
    ByteDisassembler::ByteDisassembler(const AssemblyContext&& context) :
        context(context)
    {
    }
#endif

    void ByteDisassembler::Disassemble()
    {
        const std::string& inFile { CONTEXT.InputFiles()[0] };
        std::ifstream source { System::OpenInFile(inFile) };

        Disassemble(source);
    }

    void ByteDisassembler::Disassemble(std::istream& in)
    {
        Disassemble(in, std::cout);
    }

    void ByteDisassembler::Disassemble(std::istream& in, std::ostream& out)
    {
        using namespace DisassemblerInstructions;
        using instructionFn = void(*)(std::istream& in, std::ostream& out);
        constexpr static instructionFn instructions[] {
            Nop,
            Store32, Store8, Store32Symbol, Store8Symbol,
            Load32, Load8, Read32Address, Read8Address, ReadRegister,
            MoveConstToReg, MoveStackToReg, MoveRegToReg,
            Add32, AddFloat, Add8, AddReg32, AddRegFloat, AddReg8,
            AddSafe32, AddSafeFloat, AddSafe8,
            MemCopy,
            Increment32, IncrementFloat, Increment8, IncrementReg32, IncrementRegFloat, IncrementReg8,
            IncrementSafe32, IncrementSafeFloat, IncrementSafe8,
            Decrement32, DecrementFloat, Decrement8, DecrementReg32, DecrementRegFloat, DecrementReg8,
            DecrementSafe32, DecrementSafeFloat, DecrementSafe8,
            And32, And8, AndReg,
            Or32, Or8, OrReg,
            Nor32, Nor8, NorReg,
            Swap32, Swap8, SwapReg,
            Duplicate32, Duplicate8,
            RawData, RawSymbol,
            Invert32, Invert8, InvertReg, InvertSafe32, InvertSafe8,
            CompareStack, CompareReg,
            Pop32, Pop8,
            JumpImm, JumpReg,
            SwapRange, DuplicateRange,
            Repeat, Allocate,
            PowerReg32, PowerRegFloat, PowerReg8,
            PowerStack32, PowerStackFloat, PowerStack8,
            PowerImm32, PowerImmFloat, PowerImm8,
            SquareImm32, SquareImmFloat, SquareImm8,
            SquareReg32, SquareRegFloat, SquareReg8,
            SquareStack32, SquareStackFloat, SquareStack8,
            ConditionalImm, ConditionalReg,
            CallImm, CallReg,
            Multiply32, MultiplyFloat, Multiply8,
            MultiplyReg32, MultiplyRegFloat, MultiplyReg8,
            MultiplySafe32, MultiplySafeFloat, MultiplySafe8,
            Divide32, DivideFloat, Divide8,
            DivideReg32, DivideRegFloat, DivideReg8,
            DivideSafe32, DivideSafeFloat, DivideSafe8,
            Return, Deallocate,
            Sub32, SubFloat, Sub8, SubReg32, SubRegFloat, SubReg8,
            SubSafe32, SubSafeFloat, SubSafe8,
            IncrementLocal32, IncrementLocalFloat, IncrementLocal8,
            ReadLocal32, ReadLocal8,
            CompareJump,
            CompareLocal,
            SetFlag,
            SysCall,
            Xor32, Xor8, XorReg,
        };

        AssemblyInfo info {
            "",
            0,
            nullptr,
#ifdef TOOLCHAIN_MODE
            CONTEXT
#endif
        };

        in.seekg(-sizeof(uint64_t), std::ios::end);
        uint64_t size { };
        Serialization::DeserializeInteger(size, in);
        in.seekg(-(sizeof(uint64_t)+size), std::ios::end);
        IStreamPos(in, bytecodeEnd);
        info.Deserialize(in);

        info.PrintAssemblyInfo();

        in.seekg(0, std::ios::beg);
        out << "\n.prep";
        if (info.flags & AssemblyFlags::Executable)
        {
            uint32_t dat;
            Serialization::DeserializeInteger(dat, in);
            out << "\n\torg " << dat;
            Serialization::DeserializeInteger(dat, in);
            out << "\n\tsts " << dat;
            Serialization::DeserializeInteger(dat, in);
            out << "\n\tsth " << dat;
        }
        out << "\n.body";

        for (std::streamoff pos { in.tellg() }; pos != -1 && pos < bytecodeEnd; pos = in.tellg())
        {
            char dat { };
            in.read(&dat, 1);
            instructions[static_cast<uchar_t>(dat)](in, out);
        }
        out << '\n';
    }
}
