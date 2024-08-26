#include <array>
#include <cassert>
#include <cctype>
#include <concepts>
#include <istream>
#include <ostream>
#include <string>
#include <iostream>

#include "bytemode/assembler/instructions.hpp"
#include "JASMConfig.hpp"
#include "bytemode/assembler/assembler.hpp"
#include "bytemode/assembler/modeflags.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "extensions/stringextensions.hpp"
#include "system.hpp"


namespace Instructions
{
    using namespace ByteAssembler;
    using namespace Extensions;

    namespace Reg = ModeFlags::RegisterModeFlags;
    namespace Numo = ModeFlags::NumericModeFlags;
    namespace Memo = ModeFlags::MemoryModeFlags;

    //
    // Util
    //
    template<typename T>
    concept integer = std::integral<T>;

    template<integer T>
    T _TokenToInt(const std::string& token)
    {
        try
        {
            if (token.starts_with("0x"))
                return String::HexToInt<T>(token);
            else
                return static_cast<T>(std::stoul(token));
        }
        catch (const std::exception& e)
        {
            LOGE(System::LogLevel::High, "Couldn't convert '", token, "' to '", typeid(T).name(), "'");
            return static_cast<T>(0);
        }
    }


    // I don't care about anything, I just don't want to write this
    // thing again and again
    //
    // op:
    //  0: (u)int 
    //  1: float 
    //  2: (u)byte
    void _BoringModeSwitch(const char mode, std::ostream& out, std::array<char, 3> op, std::array<std::function<void()>, 3> callbacks = {})
    {
        switch (mode)
        {
            case Numo::UInt: 
            case Numo::Int: 
                Serialization::SerializeInteger(op.at(0), out);
                if (callbacks.at(0)) callbacks.at(0)();
                break;

            case Numo::Float:
                Serialization::SerializeInteger(op.at(1), out);
                if (callbacks.at(1)) callbacks.at(1)();
                break;

            case Numo::UByte:
            case Numo::Byte:
                Serialization::SerializeInteger(op.at(2), out); 
                if (callbacks.at(2)) callbacks.at(2)();
                break;

            default:
                LOGE(System::LogLevel::High, "Unexpected mode flag '", std::to_string(mode), "'");
                break;
        }
    }

    // stackAndReg:
    //  0: stack 32-bit
    //  1: stack 8-bit
    //  2: register
    std::string _BoringLogicHandle(std::istream& in, std::ostream& out, std::array<char, 3> stackAndReg)
    {
        // [op] <mode> 
        // [op] <register> <register> 

        std::string possibleReg1 { Stream::Tokenize(in) };
        //const char possibleReg1Mode { ModeFlags::GetRegisterModeFlag(possibleReg1) };
        const char possibleReg1Mode { ModeFlags::GetModeFlag(possibleReg1, Reg::eax, Reg::sp) };

        // [op] <register> <register> 
        if (possibleReg1Mode != ModeFlags::NoMode)
        {
            //const char reg2Mode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            const char reg2Mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Reg::eax, Reg::sp, true) };

            Serialization::SerializeInteger(stackAndReg.at(1), out);
            Serialization::SerializeInteger(possibleReg1Mode, out);
            Serialization::SerializeInteger(reg2Mode, out);

            return Stream::Tokenize(in);
        }

        // [op] <mode>
        //const char mode { ModeFlags::GetModeFlag(possibleReg1, true) };
        const char mode { ModeFlags::GetModeFlag(possibleReg1, Numo::Int, Numo::UByte, true) };
        _BoringModeSwitch(mode, out, {stackAndReg.at(0), stackAndReg.at(0), stackAndReg.at(1)});

        return Stream::Tokenize(in);
    }

    // ops
    //  0: stack (u)int
    //  1: stack float
    //  2: stack (u)byte
    //
    //  3: register (u)int
    //  4: register float
    //  5: register (u)byte
    //
    // if safe, then ops:
    //  0: safe (u)int
    //  1: safe float
    //  2: safe (u)byte
    std::string _BoringArithmeticCheck(std::istream& in, std::ostream& out, std::array<char, 6> ops, bool safe = false)
    {
        if (safe) 
        {
            // [op]s <mode>
            //const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
            const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Numo::Int, Numo::UByte, true) };
            _BoringModeSwitch(mode, out, {ops.at(0), ops.at(1), ops.at(2)});

            return Stream::Tokenize(in); 
        }

        // [op] <mode>
        // [op] <mode> <register> <register>

        //const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Numo::Int, Numo::UByte, true) };
        std::string possibleReg1 { Stream::Tokenize(in) };
        //const char possibleReg1Mode { ModeFlags::GetRegisterModeFlag(possibleReg1) };
        const char possibleReg1Mode { ModeFlags::GetModeFlag(possibleReg1, Reg::eax, Reg::sp) };

        // [op] <mode> <register> <register>
        if (possibleReg1Mode != ModeFlags::NoMode)
        {
            //const char reg2Mode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            const char reg2Mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Reg::eax, Reg::sp, true) };

            _BoringModeSwitch(mode, out, {ops.at(3), ops.at(4), ops.at(5)});
            Serialization::SerializeInteger(possibleReg1Mode, out);
            Serialization::SerializeInteger(reg2Mode, out);
            return Stream::Tokenize(in);
        }

        // [op] <mode>
        _BoringModeSwitch(mode, out, {ops.at(0), ops.at(1), ops.at(2)});
        return possibleReg1;
    }


    // ops
    //  0: stack (u)int
    //  1: stack float
    //  2: stack (u)byte
    //
    //  3: register (u)int
    //  4: register float
    //  5: register (u)byte
    //
    // if safe, then ops:
    //  0: safe (u)int
    //  1: safe float
    //  2: safe (u)byte
    std::string _BoringIncDcr(std::istream& in, std::ostream& out, std::array<char, 6> ops, bool safe = false)
    {
        if (safe) 
        {
            // [op]s <mode> <value>
            //const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
            const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Numo::Int, Numo::UByte, true) };
            _BoringModeSwitch(mode, out, {ops.at(0), ops.at(1), ops.at(2)}, {
                [&in, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);},
                [&in, &out](){Serialization::SerializeFloat(std::stof(Stream::Tokenize(in)), out);},
                [&in, &out](){Serialization::SerializeInteger(_TokenToInt<char>(Stream::Tokenize(in)), out);}
            });

            return Stream::Tokenize(in);
        }

        // [op] <mode> <value>
        // [op] <mode> <register> <value>
        //const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Numo::Int, Numo::UByte, true) };
        std::string possibleReg { Stream::Tokenize(in) };
        //const char possibleRegMode { ModeFlags::GetRegisterModeFlag(possibleReg) };
        const char possibleRegMode { ModeFlags::GetModeFlag(possibleReg, Reg::eax, Reg::sp) };

        // [op] <mode> <register> <value>
        if (possibleRegMode != ModeFlags::NoMode)
        {
            namespace Regs = ModeFlags::RegisterModeFlags;
            namespace Numo = ModeFlags::NumericModeFlags;
            bool regIs8Bit { possibleRegMode >= Regs::al && possibleRegMode <= Regs::flg };

            if (regIs8Bit && (mode != Numo::Byte || mode != Numo::UByte))
                LOGE(System::LogLevel::High, "Can't use a 32-bit mode with 8-bit register ", possibleReg, ".");

            _BoringModeSwitch(mode, out, {ops.at(3), ops.at(4), ops.at(5)}, {
                [&possibleRegMode, &out, &in](){
                    Serialization::SerializeInteger(possibleRegMode, out);
                    Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);
                },
                [&possibleRegMode, &out, &in](){
                    Serialization::SerializeInteger(possibleRegMode, out);
                    Serialization::SerializeFloat(std::stof(Stream::Tokenize(in)), out);
                },
                [&possibleRegMode, &out, &in](){
                    Serialization::SerializeInteger(possibleRegMode, out);
                    Serialization::SerializeInteger(_TokenToInt<char>(Stream::Tokenize(in)), out);
                }
            });

            return Stream::Tokenize(in);
        }

        // [op] <mode> <value>
        // possibleReg is now <value>

        _BoringModeSwitch(mode, out, {ops.at(0), ops.at(1), ops.at(2)}, {
            [&possibleReg, &in, &out]() {
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(possibleReg), out);
            },
            [&possibleReg, &in, &out]() {
                Serialization::SerializeFloat(std::stof(possibleReg), out);
            },
            [&possibleReg, &in, &out]() {
                Serialization::SerializeInteger(_TokenToInt<char>(possibleReg), out);
            },
        });

        return Stream::Tokenize(in);
    }

    void _BoringRawData(std::string& token, std::istream& in, std::ostream& out)
    {
        while (token != "__JASM__ENDL__")
        {
            if (token.starts_with('"'))
            {
                token.pop_back();
                token.erase(0, 1);
                for (systembit_t i = 0; i < token.size(); i++)
                    Serialization::SerializeInteger(token.data()[i], out);

                token = Stream::Tokenize(in);
                continue;
            }

            if (!std::isdigit(token.at(0)))
                LOGE(System::LogLevel::High, "Expected a numeric constant. {", token, "}");

            if (token.find_first_of('.') != std::string::npos)
                Serialization::SerializeFloat(std::stof(token), out);
            else if ((token.find_first_of('x') != std::string::npos) && (token.size() >= 3 && token.size() <= 4))
                Serialization::SerializeInteger(_TokenToInt<char>(token), out);
            else
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(token), out);

            token = Stream::Tokenize(in);
        }
    }

    //
    // Implementation
    //
    //
    std::string Nop(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // nop
        Extensions::Serialization::SerializeInteger(OpCodes::nop, out);
        return Stream::Tokenize(in);
    }

    std::string StoreConstant(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // stc <mode> <value>
        // stc <mode> <symbol>
        // stc <float>
        // stc <integer>

        const std::string next { Stream::Tokenize(in) };
        //const char mode { ModeFlags::GetModeFlag(next) };
        const char mode { ModeFlags::GetModeFlag(next, Numo::Int, Numo::UByte) };

        // Store integer or float
        if (mode == ModeFlags::NoMode)
        {
            const char fakeFlag { (next.find_first_of('.') == std::string::npos) ? Numo::Int : Numo::Float };

            _BoringModeSwitch(fakeFlag, out, {OpCodes::stt, OpCodes::stt}, {
                [&next, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(next), out);},
                [&next, &out](){Serialization::SerializeFloat(std::stof(next), out);}
            });

            return Stream::Tokenize(in);
        }

        const std::string symOrVal { Stream::Tokenize(in) };

        // Store constant by mode
        // stc $<i/f/b> <value>
        if (std::isdigit(symOrVal.at(0)))
        {
            _BoringModeSwitch(mode, out, {OpCodes::stt, OpCodes::stt, OpCodes::ste}, {
                [&symOrVal, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(symOrVal), out); },
                [&symOrVal, &out](){Serialization::SerializeFloat(std::stof(symOrVal), out); },
                [&symOrVal, &out](){Serialization::SerializeInteger(_TokenToInt<char>(symOrVal), out); },
            });
            return Stream::Tokenize(in);
        }
            
        // Store symbol by mode
        _BoringModeSwitch(mode, out, {OpCodes::stts, OpCodes::stts, OpCodes::stes});

        size_t symbolHash { String::Hash(symOrVal) };
        if (!info.symbolMap.contains(symbolHash)) 
        {
            StreamPos(out, pos);
            info.AddUnknownSymbol(symbolHash, pos);
            Serialization::SerializeInteger<systembit_t>(0, out);
        }
        else 
            Serialization::SerializeInteger(info.symbolMap.at(symbolHash), out);

        return Stream::Tokenize(in);
    }

    std::string LoadConstant(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // ldc <mode>
        const std::string next { Stream::Tokenize(in) };
        //const char fromReg { ModeFlags::GetModeFlag(next) };
        const char mode { ModeFlags::GetModeFlag(next, Numo::Int, Numo::UByte, true) };

        _BoringModeSwitch(mode, out, {OpCodes::ldt, OpCodes::ldt, OpCodes::lde});

        return Stream::Tokenize(in);
    }

    std::string ReadAddress(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // rda <register>
        // rda <mode>

        const std::string next { Stream::Tokenize(in) };
        //char fromReg { ModeFlags::GetRegisterModeFlag(next) };
        const char possibleRegMode { ModeFlags::GetModeFlag(next, Reg::eax, Reg::sp) };
        
        // rda <mode> [read the address &ebx]
        if (possibleRegMode == ModeFlags::NoMode)
        {
            //const char mode = ModeFlags::GetModeFlag(next);
            const char mode { ModeFlags::GetModeFlag(next, Numo::Int, Numo::UByte, true) };
            _BoringModeSwitch(mode, out, {OpCodes::rdt, OpCodes::rdt, OpCodes::rde});
            return Stream::Tokenize(in);
        }

        // rda <register> [read the given register]
        //const char regMode { ModeFlags::GetRegisterModeFlag(next) };
        
        // To enforce ModeFlag
        const char regMode { ModeFlags::GetModeFlag(next, Reg::eax, Reg::sp, true) };

        Serialization::SerializeInteger(OpCodes::rdr, out);
        Serialization::SerializeInteger(regMode, out);

        return Stream::Tokenize(in);
    }

    std::string Move(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // mov <register> [from stack]
        // mov <register> <register> [from register]
        // mov <value> <register> [from constant]

        const std::string firstParam { Stream::Tokenize(in) };
        const std::string secondParam { Stream::Tokenize(in) };
        //const char firstReg { ModeFlags::GetRegisterModeFlag(firstParam) };
        //const char secondReg { ModeFlags::GetRegisterModeFlag(secondParam) };
        const char firstReg { ModeFlags::GetModeFlag(firstParam, Reg::eax, Reg::sp) };
        const char secondReg { ModeFlags::GetModeFlag(secondParam, Reg::eax, Reg::sp) };

        // move from stack
        if (secondReg == ModeFlags::NoMode)
        {
            //const char regMode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            //_BoringModeSwitch(fromReg, out, {OpCodes::movi, OpCodes::movf, OpCodes::movb});
            Serialization::SerializeInteger(OpCodes::movs, out);
            Serialization::SerializeInteger(firstReg, out);
            return secondParam;
        }

        // move from register
        if (firstReg != ModeFlags::NoMode)
        {
            // Enforcing modes
            const char firstReg { ModeFlags::GetModeFlag(firstParam, Reg::eax, Reg::sp, true) };
            const char secondReg { ModeFlags::GetModeFlag(secondParam, Reg::eax, Reg::sp, true) };
            Serialization::SerializeInteger(OpCodes::movr, out); 
            Serialization::SerializeInteger(firstReg, out);
            Serialization::SerializeInteger(secondReg, out);
            return Stream::Tokenize(in);
        }

        // move constant
        // constant size check
        // next = value token here
        //namespace Regs = ModeFlags::RegisterModeFlags;
        //const char regMode { ModeFlags::GetRegisterModeFlag(secondParam, true) };
        const char regMode { ModeFlags::GetModeFlag(secondParam, Reg::eax, Reg::sp, true) };
        bool regIs8Bit { regMode >= Reg::al && regMode <= Reg::flg };

        Serialization::SerializeInteger(OpCodes::movc, out);

        if (firstParam.find_first_of('.') == std::string::npos)
        {
            if (regIs8Bit)
                Serialization::SerializeInteger(_TokenToInt<char>(firstParam), out); 
            else
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(firstParam), out); 
        }
        else if (!regIs8Bit)
            Serialization::SerializeFloat(std::stof(firstParam), out);
        else
            LOGE(System::LogLevel::High, "Register '", secondParam, "' is too small for float value '", firstParam, "'");

        Serialization::SerializeInteger(regMode, out);

        return Stream::Tokenize(in);
    }

    std::string Add(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringArithmeticCheck(in, out, {
            OpCodes::addi, 
            OpCodes::addf, 
            OpCodes::addb, 
            OpCodes::addri, 
            OpCodes::addrf, 
            OpCodes::addrb
        });
    }

    std::string AddSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringArithmeticCheck(in, out, {
                OpCodes::addsi, 
                OpCodes::addsf, 
                OpCodes::addsb, 
            },
            true
        );
        return Stream::Tokenize(in);
    }

    std::string MemCopy(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // mcp <from> <to>
        // Serialize the <from> and <to> to one byte.
        // First 4 bits are <from>, second 4 bits are <to> 
        char fromMode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Memo::Stack, Memo::Heap, true) }; 
        char toMode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Memo::Stack, Memo::Heap, true) };

        fromMode = fromMode << 4;
        const char final { static_cast<const char>(fromMode|toMode) };

        Serialization::SerializeInteger(OpCodes::mcp, out);
        Serialization::SerializeInteger(final, out);
        
        return Stream::Tokenize(in);
    }

    std::string Increment(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringIncDcr(in, out, {
            OpCodes::inci,
            OpCodes::incf,
            OpCodes::incb,
            OpCodes::incri,
            OpCodes::incrf,
            OpCodes::incrb
        });
    }

    std::string IncrementSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringIncDcr(in, out, {
            OpCodes::incsi,
            OpCodes::incsf,
            OpCodes::incsb
        });
    }

    std::string Decrement(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringIncDcr(in, out, {
            OpCodes::dcri,
            OpCodes::dcrf,
            OpCodes::dcrb,
            OpCodes::dcrri,
            OpCodes::dcrrf,
            OpCodes::dcrrb
        });
    }

    std::string DecrementSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringIncDcr(in, out, {
            OpCodes::dcrsi,
            OpCodes::dcrsf,
            OpCodes::dcrsb
        });
    }

    std::string And(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringLogicHandle(in, out, {OpCodes::andst, OpCodes::andse, OpCodes::andr});
    }

    std::string Or(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringLogicHandle(in, out, {OpCodes::orst, OpCodes::orse, OpCodes::orr});
    }

    std::string Nor(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringLogicHandle(in, out, {OpCodes::norst, OpCodes::norse, OpCodes::norr});
    }

    std::string Swap(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // swp <mode>
        // swp <register> <register>

        std::string modeOrReg { Stream::Tokenize(in) };
        //const char possibleMode { ModeFlags::GetModeFlag(modeOrReg) };
        const char possibleMode { ModeFlags::GetModeFlag(modeOrReg, Numo::Int, Numo::UByte) };

        // swp <mode>
        if (possibleMode != ModeFlags::NoMode)
        {
            _BoringModeSwitch(possibleMode, out, {OpCodes::swpt, OpCodes::swpt, OpCodes::swpe});
            return Stream::Tokenize(in);
        }

        // swp <register> <register>
        //const char reg1 { ModeFlags::GetRegisterModeFlag(modeOrReg, true) };
        //const char reg2 { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        const char reg1 { ModeFlags::GetModeFlag(modeOrReg, Reg::eax, Reg::sp, true) };
        const char reg2 { ModeFlags::GetModeFlag(Stream::Tokenize(in), Reg::eax, Reg::sp, true) };

        Serialization::SerializeInteger(OpCodes::swpr, out);
        Serialization::SerializeInteger(reg1, out);
        Serialization::SerializeInteger(reg2, out);

        return Stream::Tokenize(in);
    }

    std::string Duplicate(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // dup <mode>
        //const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Numo::Int, Numo::UByte, true) };
        _BoringModeSwitch(mode, out, {OpCodes::dupt, OpCodes::dupt, OpCodes::dupe});

        return Stream::Tokenize(in);
    }

    std::string RawData(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // raw <..data..> ; <--- the semicolon terminates the sequence
        // raw <symbol> <size> <--- no need for semicolon
        std::string token { Stream::Tokenize(in) };

        // raw <symbol> <size>
        // because symbol names must start with a non-numeric character
        if (std::isalpha(token.at(0)))
        {
            size_t symbolHash { String::Hash(token) };

            Serialization::SerializeInteger(OpCodes::raws, out);

            if (info.symbolMap.contains(symbolHash))
                Serialization::SerializeInteger(info.symbolMap.at(symbolHash), out);
            else
            {
                StreamPos(out, pos)
                info.AddUnknownSymbol(symbolHash, pos);
                Serialization::SerializeInteger<systembit_t>(0, out);
            }

            const std::string sizeToken { Stream::Tokenize(in) };
            Serialization::SerializeInteger(_TokenToInt<systembit_t>(sizeToken), out);

            return Stream::Tokenize(in);
        }

        // raw <..data..> ;
        // This mode does NOT serialize an instruction. It directly writes data to ROM.
        Serialization::SerializeInteger(OpCodes::raw, out);
        _BoringRawData(token, in, out);
        return Stream::Tokenize(in);
    }

    std::string RomData(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // rom <..data..> ; <--- the semicolon terminates the sequence 
        std::string token { Stream::Tokenize(in) };
        _BoringRawData(token, in, out);
        return Stream::Tokenize(in);
    }
}
