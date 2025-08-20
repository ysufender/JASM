#include <array>
#include <cassert>
#include <cctype>
#include <cmath>
#include <concepts>
#include <cstdlib>
#include <exception>
#include <functional>
#include <istream>
#include <ostream>
#include <sstream>
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
            if (token.find_first_of('.') != std::string::npos)
                LOGE(System::LogLevel::High, "Can't convert the floating point token '", token, "' to an integer of type '", typeid(T).name(), "'.");

            if (token.starts_with("0x"))
                return String::HexToInt<T>(token);
            else
                return static_cast<T>(std::stoul(token));
        }
        catch (const std::exception&)
        {
            LOGE(System::LogLevel::High, "Couldn't convert type '", token, "' to '", typeid(T).name(),"'.");
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
                OStreamPos(out, errPos);
                LOGE(System::LogLevel::High, "Unexpected mode flag '", std::to_string(mode), "' at position ", std::to_string(errPos), ".");
                break;
        }
    }

    // stackAndReg:
    //  0: stack 32-bit
    //  1: stack 8-bit
    //  2: register
    std::string _BoringLogicHandle(std::istream& in, std::ostream& out, std::array<OpCodes, 3> stackAndReg)
    {
        // [op] <mode> <register>
        // [op] <register> <register> 

        std::string possibleReg1 { Stream::Tokenize(in) };
        //const uchar_t possibleReg1Mode { ModeFlags::GetRegisterModeFlag(possibleReg1) };
        const uchar_t possibleReg1Mode { ModeFlags::GetModeFlag(possibleReg1, Enumc(Reg::eax), Enumc(Reg::flg)) };
        const uchar_t reg2Mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Reg::eax), Enumc(Reg::flg), true) };

        // [op] <register> <register> 
        if (possibleReg1Mode != ModeFlags::NoMode)
        {
            //const uchar_t reg2Mode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            Serialization::SerializeInteger(stackAndReg.at(2), out);
            Serialization::SerializeInteger(possibleReg1Mode, out);
            Serialization::SerializeInteger(reg2Mode, out);

            return Stream::Tokenize(in);
        }

        // [op] <mode> <register>
        //const uchar_t mode { ModeFlags::GetModeFlag(possibleReg1, true) };
        const uchar_t mode { ModeFlags::GetModeFlag(possibleReg1, Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        _BoringModeSwitch(mode, out, {stackAndReg.at(0), stackAndReg.at(0), stackAndReg.at(1)});
        Serialization::SerializeInteger(reg2Mode, out);

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
        const uchar_t possibleReg1Mode { ModeFlags::GetModeFlag(possibleReg1, Enumc(Reg::eax), Enumc(Reg::flg)) };

        // [op] <mode> <register> <register>
        if (possibleReg1Mode != ModeFlags::NoMode)
        {
            //const uchar_t reg2Mode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            const uchar_t reg2Mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Reg::eax), Enumc(Reg::flg), true) };

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
        const uchar_t possibleRegMode { ModeFlags::GetModeFlag(possibleReg, Enumc(Reg::eax), Enumc(Reg::flg)) };

        // [op] <mode> <register> <value>
        if (possibleRegMode != ModeFlags::NoMode)
        {
            bool regIs8Bit { Is8Bit(possibleRegMode) }; 

            if (regIs8Bit && (mode != Enumc(Numo::Byte) && mode != Enumc(Numo::UByte)))
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

    void _BoringRawData(std::string& token, std::istream& in, std::ostream& out, bool rom = false)
    {
        std::stringstream ss;
        systembit_t size { 0 };
        while (token != JASM_ENDL)
        {
            if (token.starts_with('"'))
            {
                token.pop_back();
                token.erase(0, 1);
                for (systembit_t i = 0; i < token.size(); i++)
                    Serialization::SerializeInteger(token.data()[i], ss);

                size += token.size();
                token = Stream::Tokenize(in);
                continue;
            }

            if (!String::TokenIsNumber(token))
                LOGE(System::LogLevel::High, "Expected a numeric constant, got ", token);

            if (token.find_first_of('.') != std::string::npos)
            {
                Serialization::SerializeFloat(std::stof(token), ss);
                size+=4;
            }
            else if ((token.find_first_of('x') != std::string::npos) && (token.size() >= 3 && token.size() <= 4))
            {
                Serialization::SerializeInteger(_TokenToInt<uchar_t>(token), ss);
                size++;
            }
            else
            {
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(token), ss);
                size+=4;
            }

            token = Stream::Tokenize(in);
        }

        if (!rom)
            Serialization::SerializeInteger(size, out);

        out.write(ss.view().data(), size);
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
        if (String::TokenIsNumber(symOrVal))
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
            OStreamPos(out, pos);
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
        const uchar_t possibleRegMode { ModeFlags::GetModeFlag(next, Enumc(Reg::eax), Enumc(Reg::flg)) };
        
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
        const uchar_t regMode { ModeFlags::GetModeFlag(next, Enumc(Reg::eax), Enumc(Reg::flg), true) };

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
        const uchar_t firstReg { ModeFlags::GetModeFlag(firstParam, Enumc(Reg::eax), Enumc(Reg::flg)) };
        const uchar_t secondReg { ModeFlags::GetModeFlag(secondParam, Enumc(Reg::eax), Enumc(Reg::flg)) };

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
            const uchar_t firstReg { ModeFlags::GetModeFlag(firstParam, Enumc(Reg::eax), Enumc(Reg::flg), true) };
            const uchar_t secondReg { ModeFlags::GetModeFlag(secondParam, Enumc(Reg::eax), Enumc(Reg::flg), true) };
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
        const uchar_t regMode { ModeFlags::GetModeFlag(secondParam, Enumc(Reg::eax), Enumc(Reg::flg), true) };
        bool regIs8Bit { Is8Bit(regMode) }; 

        Serialization::SerializeInteger(OpCodes::movc, out);
        // first serialize regMode so at runtime we can determine if its 8 or 32 bits
        Serialization::SerializeInteger(regMode, out);

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
    
    std::string Sub(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringArithmeticCheck(in, out, {
            OpCodes::subi, 
            OpCodes::subf, 
            OpCodes::subb, 
            OpCodes::subri, 
            OpCodes::subrf, 
            OpCodes::subrb
        });
    }

    std::string SubSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringArithmeticCheck(in, out, {
                OpCodes::subsi, 
                OpCodes::subsf, 
                OpCodes::subsb, 
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
        // nor <mode>
        // nor <register> <register>
        return _BoringLogicHandle(in, out, {OpCodes::norst, OpCodes::norse, OpCodes::norr});
    }

    std::string Swap(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // swp <mode>
        // swp <register> <register>

        std::string possibleReg1 { Stream::Tokenize(in) };
        const uchar_t possibleReg1Mode { ModeFlags::GetModeFlag(possibleReg1, Enumc(Reg::eax), Enumc(Reg::flg)) };

        // swp <register> <register> 
        if (possibleReg1Mode != ModeFlags::NoMode)
        {
            const uchar_t reg2Mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Reg::eax), Enumc(Reg::flg), true) };
            Serialization::SerializeInteger(OpCodes::swpr, out);
            Serialization::SerializeInteger(possibleReg1Mode, out);
            Serialization::SerializeInteger(reg2Mode, out);

            return Stream::Tokenize(in);
        }

        // swp <mode> <register>
        const uchar_t mode { ModeFlags::GetModeFlag(possibleReg1, Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        _BoringModeSwitch(mode, out, {OpCodes::swpt, OpCodes::swpt, OpCodes::swpe});

        return Stream::Tokenize(in);

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
                OStreamPos(out, pos)
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
        _BoringRawData(token, in, out, true);
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
        const uchar_t regMode { ModeFlags::GetModeFlag(next, Enumc(Reg::eax), Enumc(Reg::flg), true) };
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
        // first 3 bits are <mode>, last 5 bits are <compare_mode>

        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        const uchar_t cmpMode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Comp::les), Enumc(Comp::neq), true) };
        const uchar_t firstThree { static_cast<const uchar_t>(mode << 5) };    
        const uchar_t compressedModes { static_cast<const uchar_t>(firstThree|cmpMode) };

        std::string possibleReg { Stream::Tokenize(in) }; 
        const uchar_t possibleReg1 { ModeFlags::GetModeFlag(possibleReg, Enumc(Reg::eax), Enumc(Reg::flg)) };

        // cmp <mode> <compare_mode>
        if (possibleReg1 == ModeFlags::NoMode)
        {
            Serialization::SerializeInteger(OpCodes::cmp, out);
            Serialization::SerializeInteger(compressedModes, out);
            return possibleReg;
        }

        // cmp <mode> <compare_mode> <register> <register>
        const uchar_t reg2 { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Reg::eax), Enumc(Reg::flg), true) };
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

    std::string Jump(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // jmp <address>
        // jmp <symbol>
        // jmp <register>

        const std::string symbolOrAddr { Stream::Tokenize(in) };
        const uchar_t possibleRegMode { ModeFlags::GetModeFlag(symbolOrAddr, Enumc(Reg::eax), Enumc(Reg::edi)) };

        // jmp <register>
        if (possibleRegMode != ModeFlags::NoMode)
        {
            Serialization::SerializeInteger(OpCodes::jmpr, out); 
            Serialization::SerializeInteger(possibleRegMode, out);

            return Stream::Tokenize(in);
        }

        Serialization::SerializeInteger(OpCodes::jmp, out);

        // jmp <address_decimal>
        // jmp <address_hex>
        if (String::TokenIsNumber(symbolOrAddr))
        {
            if (symbolOrAddr.find_first_of('.') != std::string::npos)
                LOGE(System::LogLevel::High, "Can't use floating point numbers for memory addresses.");

            Serialization::SerializeInteger(_TokenToInt<systembit_t>(symbolOrAddr), out);

            return Stream::Tokenize(in);
        }
        
        // jmp <symbol>
        const size_t symbolHash { String::Hash(symbolOrAddr) };
        if (info.symbolMap.contains(symbolHash))
            Serialization::SerializeInteger(info.symbolMap.at(symbolHash), out);
        else
        {
            OStreamPos(out, pos); 
            info.AddUnknownSymbol(symbolHash, pos);
            Serialization::SerializeInteger<systembit_t>(0, out);
        }

        return Stream::Tokenize(in);
    }

    std::string SwapRange(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // swr <size>
        const systembit_t size { _TokenToInt<systembit_t>(Stream::Tokenize(in)) };
        Serialization::SerializeInteger(OpCodes::swr, out);
        Serialization::SerializeInteger(size, out);
        return Stream::Tokenize(in);
    }

    std::string DuplicateRange(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // dur <size>
        const systembit_t size { _TokenToInt<systembit_t>(Stream::Tokenize(in)) };
        Serialization::SerializeInteger(OpCodes::dur, out);
        Serialization::SerializeInteger(size, out);
        return Stream::Tokenize(in);
    }

    std::string Repeat(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // rep <memory_mode> <mode> <count> <value>
        //
        // for heap repetitions, the address must be stored in &ebx
        
        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Memo::Stack), Enumc(Memo::Heap), true) };
        const uchar_t valueMode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte)) };
        const systembit_t count { _TokenToInt<systembit_t>(Stream::Tokenize(in)) };

        // first 4-bits is memory mode, second 4-bits is value mode 
        const uchar_t compressedModes = (mode << 4) | valueMode; 


        _BoringModeSwitch(valueMode, out, {OpCodes::rep, OpCodes::rep, OpCodes::rep}, {
            [&in, &out, compressedModes, count](){
                Serialization::SerializeInteger(compressedModes, out);
                Serialization::SerializeInteger(count, out);
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);
            }, 
            [&in, &out, compressedModes, count](){
                Serialization::SerializeInteger(compressedModes, out);
                Serialization::SerializeInteger(count, out);
                Serialization::SerializeFloat(std::stof(Stream::Tokenize(in)), out);
            },
            [&in, &out, compressedModes, count](){
                Serialization::SerializeInteger(compressedModes, out);
                Serialization::SerializeInteger(count, out);
                Serialization::SerializeInteger(_TokenToInt<uchar_t>(Stream::Tokenize(in)), out);
            }
        });

        return Stream::Tokenize(in);
    }

    std::string Allocate(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // alc
        //
        // sets &ebx to suitable address
        // size stored in &ebx

        Serialization::SerializeInteger(OpCodes::alc, out);
        return Stream::Tokenize(in);
    }

    std::string Power(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // pow <mode>
        // pow <mode> <reg> <reg> 
        // pow <mode> <constant> <constant> 

        const std::string modeString { Stream::Tokenize(in) };
        const uchar_t signedMode { ModeFlags::GetModeFlag(modeString, Enumc(Numo::Int), Enumc(Numo::Byte)) };
        if (signedMode == ModeFlags::NoMode)
            LOGE(System::LogLevel::High, "There is no pow instruction for unsigned integers.");

        // signed pow here
        const std::string regConstNone { Stream::Tokenize(in) };

        const uchar_t reg1 { ModeFlags::GetModeFlag(regConstNone, Enumc(Reg::eax), Enumc(Reg::dl)) };

        // pow <mode> <reg> <reg>
        // reg
        if (reg1 != ModeFlags::NoMode)
        {
            const std::string reg2Str { Stream::Tokenize(in) };
            const uchar_t reg2 { ModeFlags::GetModeFlag(reg2Str, Enumc(Reg::eax), Enumc(Reg::dl), true) };
            const bool cond { (Is8Bit(reg1) && Is8Bit(reg2)) || (!Is8Bit(reg1) && !Is8Bit(reg2)) && (!IsSysReg(reg1) && !IsSysReg(reg2)) };

            if (!cond)
                LOGE(System::LogLevel::High, "Given registers ", regConstNone, " and ", reg2Str, " are not suitable for pow instruction.");

            if ((Is8Bit(reg1) && signedMode != Enumc(Numo::Byte)) || (!Is8Bit(reg1) && signedMode == Enumc(Numo::Byte)))
                LOGE(System::LogLevel::High, "Size missmatch between mode and registers [", reg2Str, "].");

            _BoringModeSwitch(signedMode, out, {OpCodes::powri, OpCodes::powrf, OpCodes::powrb});
            Serialization::SerializeInteger(reg1, out);
            Serialization::SerializeInteger(reg2, out);

            return Stream::Tokenize(in);
        }

        // pow <mode> <constant> <constant>
        // constant 
        if (String::TokenIsNumber(regConstNone))
        {
            const std::string const2 { Stream::Tokenize(in) };

            _BoringModeSwitch(signedMode, out, {OpCodes::powi, OpCodes::powf, OpCodes::powb}, {
                [&in, &out, regConstNone, const2](){
                    Serialization::SerializeInteger(_TokenToInt<systembit_t>(regConstNone), out);
                    Serialization::SerializeInteger(_TokenToInt<systembit_t>(const2), out);
                },
                [&in, &out, regConstNone, const2](){
                    Serialization::SerializeFloat(std::stof(regConstNone), out);
                    Serialization::SerializeFloat(std::stof(const2), out);
                },
                [&in, &out, regConstNone, const2](){
                    Serialization::SerializeInteger(_TokenToInt<uchar_t>(regConstNone), out);
                    Serialization::SerializeInteger(_TokenToInt<uchar_t>(const2), out);
                }
            });

            return Stream::Tokenize(in);
        }

        // pow <mode>
        // stack
        _BoringModeSwitch(signedMode, out, {OpCodes::powsi, OpCodes::powsf, OpCodes::powsb});
        return regConstNone;
    }

    std::string SquareRoot(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // sqr <mode> <value> 
        // sqr <mode>
        // sqr <mode> <register>

        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::Byte), true) };
        const std::string constRegNone { Stream::Tokenize(in) };

        // sqr <mode> <number>
        if (String::TokenIsNumber(constRegNone)) 
        {
            _BoringModeSwitch(mode, out, {OpCodes::sqri, OpCodes::sqrf, OpCodes::sqrb}, {
                [&in, &out, constRegNone](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(constRegNone), out);},
                [&in, &out, constRegNone](){Serialization::SerializeFloat(std::stof(constRegNone), out);},
                [&in, &out, constRegNone](){Serialization::SerializeInteger(_TokenToInt<uchar_t>(constRegNone), out);},
            });
            
            return Stream::Tokenize(in);
        }

        const uchar_t regMode { ModeFlags::GetModeFlag(constRegNone, Enumc(Reg::eax), Enumc(Reg::flg)) };

        // sqr <mode> <register>
        if (regMode != ModeFlags::NoMode)
        {
            _BoringModeSwitch(mode, out, {OpCodes::sqrri, OpCodes::sqrrf, OpCodes::sqrrb});
            Serialization::SerializeInteger(regMode, out);

            return Stream::Tokenize(in);
        }

        // sqr <mode>
        _BoringModeSwitch(mode, out, {OpCodes::sqrsi, OpCodes::sqrsf, OpCodes::sqrsb});

        return constRegNone;
    }

    std::string Conditional(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // cnd <address>
        // cnd <symbol>
        // cnd <register>

        const std::string symbolOrAddr { Stream::Tokenize(in) };
        const uchar_t possibleRegMode { ModeFlags::GetModeFlag(symbolOrAddr, Enumc(Reg::eax), Enumc(Reg::edi)) };

        // cnd <register>
        if (possibleRegMode != ModeFlags::NoMode)
        {
            Serialization::SerializeInteger(OpCodes::cndr, out); 
            Serialization::SerializeInteger(possibleRegMode, out);

            return Stream::Tokenize(in);
        }

        Serialization::SerializeInteger(OpCodes::cnd, out);

        // cnd <address_decimal>
        // cnd <address_hex>
        if (String::TokenIsNumber(symbolOrAddr))
        {
            if (symbolOrAddr.find_first_of('.') != std::string::npos)
                LOGE(System::LogLevel::High, "Can't use floating point numbers for memory addresses.");

            Serialization::SerializeInteger(_TokenToInt<systembit_t>(symbolOrAddr), out);

            return Stream::Tokenize(in);
        }
        
        // cnd <symbol>
        const size_t symbolHash { String::Hash(symbolOrAddr) };
        if (info.symbolMap.contains(symbolHash))
            Serialization::SerializeInteger(info.symbolMap.at(symbolHash), out);
        else
        {
            OStreamPos(out, pos); 
            info.AddUnknownSymbol(symbolHash, pos);
            Serialization::SerializeInteger<systembit_t>(0, out);
        }

        return Stream::Tokenize(in);
    }

    std::string Call(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // Size should be stored on &bl
        // cal <address>
        // cal <symbol>
        // cal <register>

        const std::string symbolOrAddr { Stream::Tokenize(in) };
        const uchar_t possibleRegMode { ModeFlags::GetModeFlag(symbolOrAddr, Enumc(Reg::eax), Enumc(Reg::edi)) };

        if (possibleRegMode != ModeFlags::NoMode)
        {
            // cal <register>
            Serialization::SerializeInteger(OpCodes::calr, out); 
            Serialization::SerializeInteger(possibleRegMode, out);
        }
        else if (String::TokenIsNumber(symbolOrAddr))
        {
            // cal <address_decimal>
            // cal <address_hex>
            Serialization::SerializeInteger(OpCodes::cal, out);
            if (symbolOrAddr.find_first_of('.') != std::string::npos)
                LOGE(System::LogLevel::High, "Can't use floating point numbers for memory addresses.");

            Serialization::SerializeInteger(_TokenToInt<systembit_t>(symbolOrAddr), out);
        }
        else
        {
            // cal <symbol>
            Serialization::SerializeInteger(OpCodes::cal, out);
            const size_t symbolHash { String::Hash(symbolOrAddr) };
            if (info.symbolMap.contains(symbolHash))
                Serialization::SerializeInteger(info.symbolMap.at(symbolHash), out);
            else
            {
                OStreamPos(out, pos); 
                info.AddUnknownSymbol(symbolHash, pos);
                Serialization::SerializeInteger<systembit_t>(0, out);
            }
        }

        return Stream::Tokenize(in);
    }

    std::string Multiply(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringArithmeticCheck(in, out, {
            OpCodes::muli, 
            OpCodes::mulf, 
            OpCodes::mulb, 
            OpCodes::mulri, 
            OpCodes::mulrf, 
            OpCodes::mulrb
        });
    }

    std::string MultiplySafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringArithmeticCheck(in, out, {
                OpCodes::mulsi, 
                OpCodes::mulsf, 
                OpCodes::mulsb, 
            },
            true
        );
        return Stream::Tokenize(in);
    }

    std::string Divide(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringArithmeticCheck(in, out, {
            OpCodes::divi, 
            OpCodes::divf, 
            OpCodes::divb, 
            OpCodes::divri, 
            OpCodes::divrf, 
            OpCodes::divrb
        });
    }

    std::string DivideSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        return _BoringArithmeticCheck(in, out, {
                OpCodes::divsi, 
                OpCodes::divsf, 
                OpCodes::divsb, 
            },
            true
        );
        return Stream::Tokenize(in);
    }

    std::string Return(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        Serialization::SerializeInteger(OpCodes::ret, out);
        return Stream::Tokenize(in);
    }

    std::string Deallocate(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // del
        //
        // address is stored in &ebx
        // size is stored in &ecx

        Serialization::SerializeInteger(OpCodes::del, out);
        return Stream::Tokenize(in);
    }

    std::string IncrementLocal(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // incl <mode> <index> <value>
        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) }; 
        _BoringModeSwitch(mode, out, {OpCodes::incli, OpCodes::inclf, OpCodes::inclb}, {
            [&in, &out](){
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);
            },
            [&in, &out](){
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);
                Serialization::SerializeFloat(std::stof(Stream::Tokenize(in)), out);
            },
            [&in, &out](){
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);
                Serialization::SerializeInteger(_TokenToInt<uchar_t>(Stream::Tokenize(in)), out);
            }
        });
        return Stream::Tokenize(in);
    }

    std::string ReadLocal(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // ldl <mode> <constant>
        const std::string modeStr { Stream::Tokenize(in) };
        //const uchar_t fromReg { ModeFlags::GetModeFlag(next) };
        const uchar_t mode { ModeFlags::GetModeFlag(modeStr, Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        _BoringModeSwitch(mode, out, {OpCodes::rdlt, OpCodes::rdlt, OpCodes::rdle});
        Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);
        return Stream::Tokenize(in);
    }

    std::string ConditionalJump(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // cnj <mode> <compare_mode> <address>
        // cnj <mode> <compare_mode> <symbol>
        // cnj <mode> <compare_mode> <register>
        //
        // Serialize the <mode> and <compare_mode> to one byte
        // first 3 bits are <mode>, last 5 bits are <compare_mode>

        Serialization::SerializeInteger(OpCodes::cnj, out);

        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        const uchar_t cmpMode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Comp::les), Enumc(Comp::neq), true) };
        const uchar_t firstThree { static_cast<const uchar_t>(mode << 5) };    
        const uchar_t compressedModes { static_cast<const uchar_t>(firstThree|cmpMode) };

        Serialization::SerializeInteger(compressedModes, out);

        const std::string symbolOrAddr { Stream::Tokenize(in) };

        // cnj <compare_mode> <address>
        if (String::TokenIsNumber(symbolOrAddr))
        {
            if (symbolOrAddr.find_first_of('.') != std::string::npos)
                LOGE(System::LogLevel::High, "Can't use floating point numbers for memory addresses.");

            Serialization::SerializeInteger(_TokenToInt<systembit_t>(symbolOrAddr), out);

            return Stream::Tokenize(in);
        }
        
        // cnj <compare_mode> <symbol>
        const size_t symbolHash { String::Hash(symbolOrAddr) };
        if (info.symbolMap.contains(symbolHash))
            Serialization::SerializeInteger(info.symbolMap.at(symbolHash), out);
        else
        {
            OStreamPos(out, pos); 
            info.AddUnknownSymbol(symbolHash, pos);
            Serialization::SerializeInteger<systembit_t>(0, out);
        }

        return Stream::Tokenize(in);
    }

    std::string CompareLocal(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // cml <mode> <compare_mode> <idx1> <idx2>
        //
        // Serialize the <mode> and <compare_mode> to one byte
        // first 3 bits are <mode>, last 5 bits are <compare_mode>

        const uchar_t mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Numo::Int), Enumc(Numo::UByte), true) };
        const uchar_t cmpMode { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Comp::les), Enumc(Comp::neq), true) };
        const uchar_t firstThree { static_cast<const uchar_t>(mode << 5) };    
        const uchar_t compressedModes { static_cast<const uchar_t>(firstThree|cmpMode) };


        // cml <mode> <compare_mode> <idx1> <idx2>
        const systembit_t index1 { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Reg::eax), Enumc(Reg::flg)) };
        const systembit_t index2 { ModeFlags::GetModeFlag(Stream::Tokenize(in), Enumc(Reg::eax), Enumc(Reg::flg), true) };
        Serialization::SerializeInteger(OpCodes::cml, out);
        Serialization::SerializeInteger(compressedModes, out);
        Serialization::SerializeInteger(index1, out);
        Serialization::SerializeInteger(index2, out);

        return Stream::Tokenize(in);
    }

}
