#include <array>
#include <cassert>
#include <cctype>
#include <concepts>
#include <istream>
#include <ostream>
#include <string>
#include <iostream>

#include "bytemode/assembler/instructions.hpp"
#include "bytemode/assembler/assembler.hpp"
#include "bytemode/assembler/modeflags.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "extensions/stringextensions.hpp"
#include "JASMConfig.hpp"
#include "system.hpp"


namespace Instructions
{
    using namespace ByteAssembler;
    using namespace Extensions;

    using Reg = ModeFlags::RegisterModeFlags;
    using Numo = ModeFlags::NumericModeFlags;
    using Memo = ModeFlags::MemoryModeFlags;
    using Comp = ModeFlags::CompareModeFlags;

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
    void _BoringModeSwitch(const uchar_t mode, std::ostream& out, std::array<OpCodes, 3> op, std::array<std::function<void()>, 3> callbacks = {})
    {
        switch (mode)
        {
            case Enumc(Numo::UInt): 
            case Enumc(Numo::Int): 
                Serialization::SerializeInteger(op.at(0), out);
                if (callbacks.at(0)) callbacks.at(0)();
                break;

            case Enumc(Numo::Float):
                Serialization::SerializeInteger(op.at(1), out);
                if (callbacks.at(1)) callbacks.at(1)();
                break;

            case Enumc(Numo::UByte):
            case Enumc(Numo::Byte):
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
    std::string _BoringLogicHandle(std::istream& in, std::ostream& out, std::array<OpCodes, 3> stackAndReg)
    {
        // [op] <mode> 
        // [op] <register> <register> 

        std::string possibleReg1 { Stream::Tokenize(in) };
        //const uchar_t possibleReg1Mode { ModeFlags::GetRegisterModeFlag(possibleReg1) };
        const uchar_t possibleReg1Mode { ModeFlags::GetModeFlag(possibleReg1, Enumc(Reg::eax), Enumc(Reg::sp)) };

        // [op] <register> <register> 
        if (possibleReg1Mode != ModeFlags::NoMode)
        {
            //const uchar_t reg2Mode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            const uchar_t reg2Mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Reg::eax), Enumc(Reg::sp), true) };

            Serialization::SerializeInteger(stackAndReg.at(1), out);
            Serialization::SerializeInteger(possibleReg1Mode, out);
            Serialization::SerializeInteger(reg2Mode, out);

            return Stream::Tokenize(in);
        }

        // [op] <mode>
        //const uchar_t mode { ModeFlags::GetModeFlag(possibleReg1, true) };
        const uchar_t mode { ModeFlags::GetModeFlag(possibleReg1, Enumc(Numo::Int), Enumc(Numo::UByte), true) };
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
    std::string _BoringArithmeticCheck(std::istream& in, std::ostream& out, std::array<OpCodes, 6> ops, bool safe = false)
    {
        if (safe) 
        {
            // [op]s <mode>
            //const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
            const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
            _BoringModeSwitch(mode, out, {ops.at(0), ops.at(1), ops.at(2)});

            return Stream::Tokenize(in); 
        }

        // [op] <mode>
        // [op] <mode> <register> <register>

        //const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        std::string possibleReg1 { Stream::Tokenize(in) };
        //const uchar_t possibleReg1Mode { ModeFlags::GetRegisterModeFlag(possibleReg1) };
        const uchar_t possibleReg1Mode { ModeFlags::GetModeFlag(possibleReg1, Enumc(Reg::eax), Enumc(Reg::sp)) };

        // [op] <mode> <register> <register>
        if (possibleReg1Mode != ModeFlags::NoMode)
        {
            //const uchar_t reg2Mode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            const uchar_t reg2Mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Reg::eax), Enumc(Reg::sp), true) };

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
    std::string _BoringIncDcr(std::istream& in, std::ostream& out, std::array<OpCodes, 6> ops, bool safe = false)
    {
        if (safe) 
        {
            // [op]s <mode> <value>
            //const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
            const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
            _BoringModeSwitch(mode, out, {ops.at(0), ops.at(1), ops.at(2)}, {
                [&in, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);},
                [&in, &out](){Serialization::SerializeFloat(std::stof(Stream::Tokenize(in)), out);},
                [&in, &out](){Serialization::SerializeInteger(_TokenToInt<uchar_t>(Stream::Tokenize(in)), out);}
            });

            return Stream::Tokenize(in);
        }

        // [op] <mode> <value>
        // [op] <mode> <register> <value>
        //const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        std::string possibleReg { Stream::Tokenize(in) };
        //const uchar_t possibleRegMode { ModeFlags::GetRegisterModeFlag(possibleReg) };
        const uchar_t possibleRegMode { ModeFlags::GetModeFlag(possibleReg, Enumc(Reg::eax), Enumc(Reg::sp)) };

        // [op] <mode> <register> <value>
        if (possibleRegMode != ModeFlags::NoMode)
        {
            bool regIs8Bit { Is8Bit(possibleRegMode) }; 

            if (regIs8Bit && (mode != Enumc(Numo::Byte) || mode != Enumc(Numo::UByte)))
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
                    Serialization::SerializeInteger(_TokenToInt<uchar_t>(Stream::Tokenize(in)), out);
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
                Serialization::SerializeInteger(_TokenToInt<uchar_t>(possibleReg), out);
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
                Serialization::SerializeInteger(_TokenToInt<uchar_t>(token), out);
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
        //const uchar_t mode { ModeFlags::GetModeFlag(next) };
        const uchar_t mode { ModeFlags::GetModeFlag(next, Enumc(Numo::Int), Enumc(Numo::UByte)) };

        // Store integer or float
        if (mode == ModeFlags::NoMode)
        {
            const uchar_t fakeFlag { (next.find_first_of('.') == std::string::npos) ? Enumc(Numo::Int) : Enumc(Numo::Float) };

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
                [&symOrVal, &out](){Serialization::SerializeInteger(_TokenToInt<uchar_t>(symOrVal), out); },
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
        //const uchar_t fromReg { ModeFlags::GetModeFlag(next) };
        const uchar_t mode { ModeFlags::GetModeFlag(next, Enumc(Numo::Int), Enumc(Numo::UByte), true) };

        _BoringModeSwitch(mode, out, {OpCodes::ldt, OpCodes::ldt, OpCodes::lde});

        return Stream::Tokenize(in);
    }

    std::string ReadAddress(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // rda <register>
        // rda <mode>

        const std::string next { Stream::Tokenize(in) };
        //uchar_t fromReg { ModeFlags::GetRegisterModeFlag(next) };
        const uchar_t possibleRegMode { ModeFlags::GetModeFlag(next, Enumc(Reg::eax), Enumc(Reg::sp)) };
        
        // rda <mode> [read the address &ebx]
        if (possibleRegMode == ModeFlags::NoMode)
        {
            //const uchar_t mode = ModeFlags::GetModeFlag(next);
            const uchar_t mode { ModeFlags::GetModeFlag(next, Enumc(Numo::Int), Enumc(Numo::UByte), true) };
            _BoringModeSwitch(mode, out, {OpCodes::rdt, OpCodes::rdt, OpCodes::rde});
            return Stream::Tokenize(in);
        }

        // rda <register> [read the given register]
        //const uchar_t regMode { ModeFlags::GetRegisterModeFlag(next) };
        
        // To enforce ModeFlag
        const uchar_t regMode { ModeFlags::GetModeFlag(next, Enumc(Reg::eax), Enumc(Reg::sp), true) };

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
        //const uchar_t firstReg { ModeFlags::GetRegisterModeFlag(firstParam) };
        //const uchar_t secondReg { ModeFlags::GetRegisterModeFlag(secondParam) };
        const uchar_t firstReg { ModeFlags::GetModeFlag(firstParam, Enumc(Reg::eax), Enumc(Reg::sp)) };
        const uchar_t secondReg { ModeFlags::GetModeFlag(secondParam, Enumc(Reg::eax), Enumc(Reg::sp)) };

        // move from stack
        if (secondReg == ModeFlags::NoMode)
        {
            //const uchar_t regMode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            //_BoringModeSwitch(fromReg, out, {OpCodes::movi, OpCodes::movf, OpCodes::movb});
            Serialization::SerializeInteger(OpCodes::movs, out);
            Serialization::SerializeInteger(firstReg, out);
            return secondParam;
        }

        // move from register
        if (firstReg != ModeFlags::NoMode)
        {
            // Enforcing modes
            const uchar_t firstReg { ModeFlags::GetModeFlag(firstParam, Enumc(Reg::eax), Enumc(Reg::sp), true) };
            const uchar_t secondReg { ModeFlags::GetModeFlag(secondParam, Enumc(Reg::eax), Enumc(Reg::sp), true) };
            Serialization::SerializeInteger(OpCodes::movr, out); 
            Serialization::SerializeInteger(firstReg, out);
            Serialization::SerializeInteger(secondReg, out);
            return Stream::Tokenize(in);
        }

        // move constant
        // constant size check
        // next = value token here
        //namespace Regs = ModeFlags::RegisterModeFlags;
        //const uchar_t regMode { ModeFlags::GetRegisterModeFlag(secondParam, true) };
        const uchar_t regMode { ModeFlags::GetModeFlag(secondParam, Enumc(Reg::eax), Enumc(Reg::sp), true) };
        bool regIs8Bit { Is8Bit(regMode) }; 

        Serialization::SerializeInteger(OpCodes::movc, out);

        if (firstParam.find_first_of('.') == std::string::npos)
        {
            if (regIs8Bit)
                Serialization::SerializeInteger(_TokenToInt<uchar_t>(firstParam), out); 
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
        uchar_t fromMode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Memo::Stack), Enumc(Memo::Heap), true) }; 
        uchar_t toMode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Memo::Stack), Enumc(Memo::Heap), true) };

        fromMode = fromMode << 4;
        const uchar_t final { static_cast<const uchar_t>(fromMode|toMode) };

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
        // and <mode>
        // and <register> <register> 
        return _BoringLogicHandle(in, out, {OpCodes::andst, OpCodes::andse, OpCodes::andr});
    }

    std::string Or(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // or <mode>
        // or <register> <register>
        return _BoringLogicHandle(in, out, {OpCodes::orst, OpCodes::orse, OpCodes::orr});
    }

    std::string Nor(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // or <mode>
        // or <register> <register>
        return _BoringLogicHandle(in, out, {OpCodes::norst, OpCodes::norse, OpCodes::norr});
    }

    std::string Swap(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // swp <mode>
        // swp <register> <register>
        return _BoringLogicHandle(in, out, {OpCodes::swpt, OpCodes::swpe, OpCodes::swpr});
    }

    std::string Duplicate(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // dup <mode>
        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
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
        Serialization::SerializeInteger(OpCodes::raw, out);
        _BoringRawData(token, in, out);
        return Stream::Tokenize(in);
    }

    std::string RomData(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // rom <..data..> ; <--- the semicolon terminates the sequence 
        // This mode does NOT serialize an instruction. It directly writes data to ROM.
        std::string token { Stream::Tokenize(in) };
        _BoringRawData(token, in, out);
        return Stream::Tokenize(in);
    }

    std::string Invert(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // inv <mode> 
        // inv <register>
        
        const std::string next { Stream::Tokenize(in) };
        const uchar_t possibleMode { ModeFlags::GetModeFlag(next, Enumc(Numo::Int), Enumc(Numo::UByte)) };

        // inv <mode>
        if (possibleMode != ModeFlags::NoMode)
        {
            _BoringModeSwitch(possibleMode, out, {OpCodes::invt, OpCodes::invt, OpCodes::inve});
            return Stream::Tokenize(in);
        }

        // inv <register>
        const uchar_t regMode { ModeFlags::GetModeFlag(next, Enumc(Reg::eax), Enumc(Reg::sp), true) };
        Serialization::SerializeInteger(OpCodes::invr, out);
        Serialization::SerializeInteger(regMode, out);
        return Stream::Tokenize(in);
    }

    std::string InvertSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // invs <mode>
        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        _BoringModeSwitch(mode, out, {OpCodes::invst, OpCodes::invst, OpCodes::invse});
        return Stream::Tokenize(in);
    }

    std::string Compare(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // cmp <mode> <compare_mode>
        // cmp <mode> <compare_mode> <register> <register>
        //
        // Serialize the <mode> and <compare_mode> to one byte
        // first 3 bits are <mode>, 5 bits are <compare_mode>

        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        const uchar_t cmpMode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Comp::les), Enumc(Comp::neq), true) };
        const uchar_t firstThree { static_cast<const uchar_t>(mode << 5) };    
        const uchar_t compressedModes { static_cast<const uchar_t>(firstThree|cmpMode) };

        std::string possibleReg { Stream::Tokenize(in) }; 
        const uchar_t possibleReg1 { ModeFlags::GetModeFlag(possibleReg, Enumc(Reg::eax), Enumc(Reg::sp)) };

        // cmp <mode> <compare_mode>
        if (possibleReg1 == ModeFlags::NoMode)
        {
            Serialization::SerializeInteger(OpCodes::cmp, out);
            Serialization::SerializeInteger(compressedModes, out);
            return possibleReg;
        }

        // cmp <mode> <compare_mode> <register> <register>
        const uchar_t reg2 { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Reg::eax), Enumc(Reg::sp), true) };
        Serialization::SerializeInteger(OpCodes::cmpr, out);
        Serialization::SerializeInteger(compressedModes, out);
        Serialization::SerializeInteger(possibleReg1, out);
        Serialization::SerializeInteger(reg2, out);

        return Stream::Tokenize(in);
    }

    std::string Pop(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // pop <mode> 
        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        _BoringModeSwitch(mode, out, {OpCodes::popt, OpCodes::popt, OpCodes::pope});
        return Stream::Tokenize(in);
    }
}
