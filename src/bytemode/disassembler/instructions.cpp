#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <istream>
#include <iostream>

#include "JASMConfig.hpp"
#include "bytemode/assembler/modeflags.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "extensions/stringextensions.hpp"

using Reg = ModeFlags::RegisterModeFlags;
using Numo = ModeFlags::NumericModeFlags;
using Memo = ModeFlags::MemoryModeFlags;
using Comp = ModeFlags::CompareModeFlags;

using namespace Extensions::Serialization;

std::unordered_map<uchar_t, std::string_view> modeStr {
    // Shouldn't use the NumericModeFlags. Or SetFlag won't work.

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

    {Extensions::String::ConstHash("syscall"), "syscall"},
    {Extensions::String::ConstHash("on"), "on"},
    {Extensions::String::ConstHash("off"), "off"},
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
            uchar_t val;
            DeserializeInteger(val, in);
            out << "\nmov " << static_cast<int>(val) << " " << modeStr.at(static_cast<uchar_t>(regMode));
        }
        else
        {
            systembit_t val;
            DeserializeInteger(val, in);
            out << "\nmov " << val << " " << modeStr.at(static_cast<uchar_t>(regMode));
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
            << " "
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
            << " "
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
            << " "
            << modeStr.at(rRegMode);
    }

    void Swap32(std::istream& in, std::ostream& out)
    {
        out << "\nswp %i/%ui";
    }

    void Swap8(std::istream& in, std::ostream& out)
    {
        out << "\nswp %b/%ub";
    }

    void SwapReg(std::istream& in, std::ostream& out)
    {
        uchar_t fromReg, toReg;
        DeserializeInteger(fromReg, in);
        DeserializeInteger(toReg, in);

        out << "\nswp "
            << modeStr.at(fromReg)
            << " "
            << modeStr.at(toReg);
    }

    void Duplicate32(std::istream& in, std::ostream& out)
    {
        out << "\nswp %i/%ui";
    }

    void Duplicate8(std::istream& in, std::ostream& out)
    {
        out << "\nswp %b/%ub";
    }

    void RawData(std::istream& in, std::ostream& out)
    {
        systembit_t size;
        DeserializeInteger(size, in);

        out << "\nraw ";
        for (; size > 0; size--) {
            const char ch { static_cast<char>(in.get()) };

            if (isalpha(ch)) {
                out << ch;
            }
            else {
                out << " 0x" << std::hex << static_cast<int>(ch) << ' '; 
            }
        }
        out << ';';
    }

    void RawSymbol(std::istream& in, std::ostream& out)
    {
        systembit_t symbolHash, size;
        DeserializeInteger(symbolHash, in);
        DeserializeInteger(size, in);

        out << "\nraw symbol@" << symbolHash << " " << size;
    }

    // This instruction can't be detected since it leaves no trace
    // void RomData(std::istream& in, std::ostream& out)
    // {
    //
    // }

    void Invert32(std::istream& in, std::ostream& out)
    {
        out << "\ninv ½i/%ui";
    }

    void Invert8(std::istream& in, std::ostream& out)
    {
        out << "\ninv ½b/%ub";
    }

    void InvertReg(std::istream& in, std::ostream& out)
    {
        uchar_t reg;
        DeserializeInteger(reg, in);

        out << "\ninv " << modeStr.at(reg);
    }

    void InvertSafe32(std::istream& in, std::ostream& out)
    {
        out << "\ninv ½i/%ui";
    }

    void InvertSafe8(std::istream& in, std::ostream& out)
    {
        out << "\ninv ½b/%ub";
    }
    
    void CompareStack(std::istream& in, std::ostream& out)
    {
        uchar_t compressed;
        DeserializeInteger(compressed, in);

        uchar_t
            numMode = compressed >> 5,
            compareMode = (compressed << 5) >> 5;

        out << "\ncmp "
            << modeStr.at(numMode)
            << " "
            << modeStr.at(compareMode);
    }

    void CompareReg(std::istream& in, std::ostream& out)
    {
        uchar_t compressed;
        DeserializeInteger(compressed, in);

        uchar_t
            numMode = compressed >> 5,
            compareMode = (compressed << 5) >> 5;

        uchar_t lhs, rhs;
        DeserializeInteger(lhs, in);
        DeserializeInteger(rhs, in);

        out << "\ncmp "
            << modeStr.at(numMode)
            << " "
            << modeStr.at(compareMode)
            << " "
            << modeStr.at(lhs)
            << " "
            << modeStr.at(rhs);
    }

    void Pop32(std::istream& in, std::ostream& out)
    {
        out << "\npop %i/%ui";
    }

    void Pop8(std::istream& in, std::ostream& out)
    {
        out << "\npop %b/%ub";
    }

    void JumpImm(std::istream& in, std::ostream& out)
    {
        systembit_t address;
        DeserializeInteger(address, in);

        out << "\njmp " << address;
    }

    void JumpReg(std::istream& in, std::ostream& out)
    {
        uchar_t reg;
        DeserializeInteger(reg, in);

        out << "\njmp " << modeStr.at(reg);
    }

    void SwapRange(std::istream& in, std::ostream& out)
    {
        systembit_t size;
        DeserializeInteger(size, in);

        out << "\nswr " << size;
    }

    void DuplicateRange(std::istream& in, std::ostream& out)
    {
        systembit_t size;
        DeserializeInteger(size, in);

        out << "\ndur " << size;
    }

    void Repeat(std::istream& in, std::ostream& out)
    {
        uchar_t compressedModes;
        DeserializeInteger(compressedModes, in);

        const uchar_t memoryModeInt { static_cast<const uchar_t>((compressedModes & 0b11110000) >> 4) };
        const uchar_t valueModeInt { static_cast<const uchar_t>(compressedModes & 0b00001111) };

        const ModeFlags::RegisterModeFlags memoryMode { memoryModeInt };
        const ModeFlags::NumericModeFlags valueMode { valueModeInt };

        systembit_t count;
        DeserializeInteger(count, in);

        out << "\nrep "
            << modeStr.at(memoryModeInt)
            << " "
            << modeStr.at(valueModeInt)
            << " ";

        switch (valueMode) {
            case ModeFlags::NumericModeFlags::UInt:
            case ModeFlags::NumericModeFlags::Int: {
                systembit_t value;
                DeserializeInteger(value, in);
                out << value;
            }
            
            case ModeFlags::NumericModeFlags::UByte:
            case ModeFlags::NumericModeFlags::Byte: {
                uchar_t value;
                DeserializeInteger(value, in);
                out << value;
            }

            case ModeFlags::NumericModeFlags::Float: {
                float value;
                DeserializeFloat(value, in);
                out << value;
            }
        }
    }

    void Allocate(std::istream& in, std::ostream& out)
    {
        out << "\nalc";
    }

    void PowerImm32(std::istream& in, std::ostream& out)
    {
        systembit_t base, power;
        DeserializeInteger(base, in);
        DeserializeInteger(power, in);

        out << "\npow %i/%ui "
            << base
            << " "
            << power;
    }

    void PowerImm8(std::istream& in, std::ostream& out)
    {
        uchar_t base, power;
        DeserializeInteger(base, in);
        DeserializeInteger(power, in);

        out << "\npow %b/%ub "
            << base
            << " "
            << power;
    }

    void PowerImmFloat(std::istream& in, std::ostream& out)
    {
        float base, power;
        DeserializeFloat(base, in);
        DeserializeFloat(power, in);

        out << "\npow %f "
            << base
            << " "
            << power;
    }

    void PowerStack32(std::istream& in, std::ostream& out)
    {
        out << "\npow %i/%ui";
    }

    void PowerStack8(std::istream& in, std::ostream& out)
    {
        out << "\npow %b/%ub";
    }

    void PowerStackFloat(std::istream& in, std::ostream& out)
    {
        out << "\npow %f";
    }

    void PowerReg32(std::istream& in, std::ostream& out)
    {
        uchar_t base, power;
        DeserializeInteger(base, in);
        DeserializeInteger(power, in);

        out << "\npow %i/%ui "
            << modeStr.at(base)
            << " "
            << modeStr.at(power);
    }

    void PowerReg8(std::istream& in, std::ostream& out)
    {
        uchar_t base, power;
        DeserializeInteger(base, in);
        DeserializeInteger(power, in);

        out << "\npow %b/%ub "
            << modeStr.at(base)
            << " "
            << modeStr.at(power);
    }

    void PowerRegFloat(std::istream& in, std::ostream& out)
    {
        uchar_t base, power;
        DeserializeInteger(base, in);
        DeserializeInteger(power, in);

        out << "\npow %f "
            << modeStr.at(base)
            << " "
            << modeStr.at(power);
    }

    void SquareImm32(std::istream& in, std::ostream& out)
    {
        systembit_t value;
        DeserializeInteger(value, in);

        out << "\nsqr %i/%ui " << value;
    }

    void SquareImm8(std::istream& in, std::ostream& out)
    {
        uchar_t value;
        DeserializeInteger(value, in);

        out << "\nsqr %b/%ub " << value;
    }

    void SquareImmFloat(std::istream& in, std::ostream& out)
    {
        float value;
        DeserializeFloat(value, in);

        out << "\nsqr %f " << value;
    }

    void SquareStack32(std::istream& in, std::ostream& out)
    {
        out << "\nsqr %i/%ui";
    }

    void SquareStack8(std::istream& in, std::ostream& out)
    {
        out << "\nsqr %b/%ub";
    }

    void SquareStackFloat(std::istream& in, std::ostream& out)
    {
        out << "\nsqr %f";
    }

    void SquareReg32(std::istream& in, std::ostream& out)
    {
        uchar_t reg;
        DeserializeInteger(reg, in);

        out << "\nsqr %i/%ui " << modeStr.at(reg);
    }

    void SquareReg8(std::istream& in, std::ostream& out)
    {
        uchar_t reg;
        DeserializeInteger(reg, in);

        out << "\nsqr %b/%ub " << modeStr.at(reg);
    }

    void SquareRegFloat(std::istream& in, std::ostream& out)
    {
        uchar_t reg;
        DeserializeInteger(reg, in);

        out << "\nsqr %f " << modeStr.at(reg);
    }

    void ConditionalImm(std::istream& in, std::ostream& out)
    {
        systembit_t addr;
        DeserializeInteger(addr, in);

        out << "\ncnd " << addr;
    }

    void ConditionalReg(std::istream& in, std::ostream& out)
    {
        uchar_t reg;
        DeserializeInteger(reg, in);

        out << "\ncnd " << modeStr.at(reg);
    }

    void CallImm(std::istream& in, std::ostream& out)
    {
        systembit_t addr;
        DeserializeInteger(addr, in);

        out << "\ncal " << modeStr.at(addr);
    }

    void CallReg(std::istream& in, std::ostream& out)
    {
        uchar_t reg;
        DeserializeInteger(reg, in);

        out << "\ncal " << modeStr.at(reg);
    }

    void Multiply32(std::istream& in, std::ostream& out)
    {
        out << "\nmul %i/%ui";
    }

    void Multiply8(std::istream& in, std::ostream& out)
    {
        out << "\nmul %b/%ub";
    }

    void MultiplyFloat(std::istream& in, std::ostream& out)
    {
        out << "\nmul %f";
    }

    void MultiplyReg32(std::istream& in, std::ostream& out)
    {
        uchar_t reg1, reg2;
        DeserializeInteger(reg1, in);
        DeserializeInteger(reg2, in);

        out << "\nmul %i/%ui "
            << modeStr.at(reg1) 
            << " "
            << modeStr.at(reg2);
    }

    void MultiplyReg8(std::istream& in, std::ostream& out)
    {
        uchar_t reg1, reg2;
        DeserializeInteger(reg1, in);
        DeserializeInteger(reg2, in);

        out << "\nmul %b/%ub "
            << modeStr.at(reg1) 
            << " "
            << modeStr.at(reg2);
    }

    void MultiplyRegFloat(std::istream& in, std::ostream& out)
    {
        uchar_t reg1, reg2;
        DeserializeInteger(reg1, in);
        DeserializeInteger(reg2, in);

        out << "\nmul %f "
            << modeStr.at(reg1) 
            << " "
            << modeStr.at(reg2);
    }

    void MultiplySafe32(std::istream& in, std::ostream& out)
    {
        out << "\nmuls %i/%ui";
    }

    void MultiplySafe8(std::istream& in, std::ostream& out)
    {
        out << "\nmuls %b/%ub";
    }

    void MultiplySafeFloat(std::istream& in, std::ostream& out)
    {
        out << "\nmuls %f";
    }

    void Divide32(std::istream& in, std::ostream& out)
    {
        out << "\ndiv %i/%ui";
    }

    void Divide8(std::istream& in, std::ostream& out)
    {
        out << "\ndiv %b/%ub";
    }

    void DivideFloat(std::istream& in, std::ostream& out)
    {
        out << "\ndiv %f";
    }

    void DivideReg32(std::istream& in, std::ostream& out)
    {
        uchar_t reg1, reg2;
        DeserializeInteger(reg1, in);
        DeserializeInteger(reg2, in);

        out << "\ndiv %i/%ui "
            << modeStr.at(reg1) 
            << " "
            << modeStr.at(reg2);
    }

    void DivideReg8(std::istream& in, std::ostream& out)
    {
        uchar_t reg1, reg2;
        DeserializeInteger(reg1, in);
        DeserializeInteger(reg2, in);

        out << "\ndiv %b/%ub "
            << modeStr.at(reg1) 
            << " "
            << modeStr.at(reg2);
    }

    void DivideRegFloat(std::istream& in, std::ostream& out)
    {
        uchar_t reg1, reg2;
        DeserializeInteger(reg1, in);
        DeserializeInteger(reg2, in);

        out << "\ndiv %f "
            << modeStr.at(reg1) 
            << " "
            << modeStr.at(reg2);
    }

    void DivideSafe32(std::istream& in, std::ostream& out)
    {
        out << "\ndivs %i/%ui";
    }

    void DivideSafe8(std::istream& in, std::ostream& out)
    {
        out << "\ndivs %b/%ub";
    }

    void DivideSafeFloat(std::istream& in, std::ostream& out)
    {
        out << "\ndivs %f";
    }

    void Return(std::istream& in, std::ostream& out)
    {
        out << "\nret";
    }

    void Deallocate(std::istream& in, std::ostream& out)
    {
        out << "\ndel";
    }

    void IncrementLocal32(std::istream& in, std::ostream& out)
    {
        systembit_t index, value;
        DeserializeInteger(index, in);
        DeserializeInteger(value, in);

        out << "\nincl %i/%ui " << index << " " << value;
    }

    void IncrementLocal8(std::istream& in, std::ostream& out)
    {
        systembit_t index;
        uchar_t value;
        DeserializeInteger(index, in);
        DeserializeInteger(value, in);

        out << "\nincl %b/%ub " << index << " " << value;
    }

    void IncrementLocalFloat(std::istream& in, std::ostream& out)
    {
        systembit_t index;
        float value;
        DeserializeInteger(index, in);
        DeserializeFloat(value, in);

        out << "\nincl %f " << index << " " << value;
    }

    void ReadLocal32(std::istream& in, std::ostream& out)
    {
        systembit_t index;
        DeserializeInteger(index, in);

        out << "\nrdl %i/%ui/%f " << index;
    }

    void ReadLocal8(std::istream& in, std::ostream& out)
    {
        systembit_t index;
        DeserializeInteger(index, in);

        out << "\nrdl %b/%ub " << index;
    }

    void CompareJump(std::istream& in, std::ostream& out)
    {
        uchar_t compressed;
        DeserializeInteger(compressed, in);

        const uchar_t valueMode { static_cast<const uchar_t>((compressed & 0b11100000) >> 5) };
        const uchar_t compareMode { static_cast<const uchar_t>(compressed & ~0b11100000) };

        systembit_t addr;
        DeserializeInteger(addr, in);

        out << "\ncnj "
            << modeStr.at(valueMode)
            << " "
            << modeStr.at(compareMode)
            << " "
            << addr;
    }

    void CompareLocal(std::istream& in, std::ostream& out)
    {
        uchar_t compressed;
        DeserializeInteger(compressed, in);

        const uchar_t valueMode { static_cast<const uchar_t>((compressed & 0b11100000) >> 5) };
        const uchar_t compareMode { static_cast<const uchar_t>(compressed & ~0b11100000) };

        systembit_t idx1, idx2;
        DeserializeInteger(idx1, in);
        DeserializeInteger(idx2, in);

        out << "\ncml "
            << modeStr.at(valueMode)
            << " "
            << modeStr.at(compareMode)
            << " "
            << idx1
            << " "
            << idx2;
    }

    // Should be unreachable
    // void SymbolAddres(std::istream& in, std::ostream& out)
    // {
    // }

    void SetFlag(std::istream& in, std::ostream& out)
    {
        // Must be kept in sync with the assembler/instructions.cpp

        uchar_t compressed;
        DeserializeInteger(compressed, in);

        const uchar_t flag { static_cast<const uchar_t>((compressed & 0b11110000) >> 4) };
        const uchar_t value { static_cast<const uchar_t>(compressed & ~0b11110000) };

        out << "\nstf " << modeStr.at(flag) << " " << modeStr.at(value);
    }

    void SysCall(std::istream& in, std::ostream& out)
    {
        systembit_t romAddr;
        DeserializeInteger(romAddr, in);

        IStreamPos(in, current);

        in.seekg(romAddr);

        systembit_t size;
        DeserializeInteger(size, in);

        out << "\nsys \"";
        for (; size > 0; size--)
            out << char(in.get());
        out << "\"";

        in.seekg(current);
    }

    void Xor32(std::istream& in, std::ostream& out)
    {
        out << "\nunsupported";
    }

    void Xor8(std::istream& in, std::ostream& out)
    {
        out << "\nunsupported";
    }

    void XorReg(std::istream& in, std::ostream& out)
    {
        out << "\nunsupported";
    }
}
