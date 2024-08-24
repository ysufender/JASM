#include <array>
#include <cassert>
#include <cctype>
#include <concepts>
#include <fstream>
#include <ios>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <iostream>

#include "bytemode/assembler/instructions.hpp"
#include "JASMConfig.hpp"
#include "bytemode/assembler/modeflags.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "extensions/stringextensions.hpp"
#include "extensions/syntaxextensions.hpp"
#include "system.hpp"


namespace Instructions
{
    using namespace ByteAssembler;

    //
    // Util
    //
    using namespace Extensions;

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
    void _BoringModeSwitch(const char mode, std::ostream& out, std::array<char, 3> op, std::array<std::function<void()>, 3> callbacks = {})
    {
        namespace NModes = ModeFlags::NumericModeFlags;

        switch (mode)
        {
            case NModes::UInt: 
            case NModes::Int: 
                Serialization::SerializeInteger(op.at(0), out);
                if (callbacks.at(0)) callbacks.at(0)();
                break;

            case NModes::Float:
                Serialization::SerializeInteger(op.at(1), out);
                if (callbacks.at(1)) callbacks.at(1)();
                break;

            case NModes::UByte:
            case NModes::Byte:
                Serialization::SerializeInteger(op.at(2), out); 
                if (callbacks.at(2)) callbacks.at(2)();
                break;

            default:
                LOGE(System::LogLevel::High, "Unexpected mode flag '", std::to_string(mode), "'");
                break;
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
        const char fromReg { ModeFlags::GetModeFlag(next) };

        // Store integer or float
        if (fromReg == ModeFlags::NoMode)
        {
            namespace NMode = ModeFlags::NumericModeFlags;

            const char fakeFlag { next.find_first_of('.' == std::string::npos) ? NMode::Int : NMode::Float };

            _BoringModeSwitch(fakeFlag, out, {OpCodes::sti, OpCodes::stf, OpCodes::nop}, {
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
            _BoringModeSwitch(fromReg, out, {OpCodes::sti, OpCodes::stf, OpCodes::stb}, {
                [&symOrVal, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(symOrVal), out); },
                [&symOrVal, &out](){Serialization::SerializeFloat(std::stof(symOrVal), out); },
                [&symOrVal, &out](){Serialization::SerializeInteger(_TokenToInt<char>(symOrVal), out); },
            });
            return Stream::Tokenize(in);
        }
            
        // Store symbol by mode
        _BoringModeSwitch(fromReg, out, {OpCodes::stis, OpCodes::stfs, OpCodes::stbs});

        size_t symbolHash { String::Hash(symOrVal) };
        if (!info.symbolMap.contains(symbolHash)) 
        {
            info.unknownSymbols.push_back({symbolHash, static_cast<systembit_t>(out.tellp())});
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
        const char fromReg { ModeFlags::GetModeFlag(next) };

        _BoringModeSwitch(fromReg, out, {OpCodes::ldi, OpCodes::ldf, OpCodes::ldb}, { });

        return Stream::Tokenize(in);
    }

    std::string ReadAddress(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // rda <register>
        // rda <mode>

        const std::string next { Stream::Tokenize(in) };
        char fromReg { ModeFlags::GetRegisterModeFlag(next) };
        
        // rda <mode> [read the address &ebx]
        if (fromReg == ModeFlags::NoMode)
        {
            fromReg = ModeFlags::GetModeFlag(next);
            _BoringModeSwitch(fromReg, out, {OpCodes::rdi, OpCodes::rdf, OpCodes::rdb});
            return Stream::Tokenize(in);
        }

        // rda <register> [read the given register]
        const char regMode { ModeFlags::GetRegisterModeFlag(next) };
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
        const char firstReg { ModeFlags::GetRegisterModeFlag(firstParam) };
        const char secondReg { ModeFlags::GetRegisterModeFlag(secondParam) };

        // move from stack
        if (secondReg == ModeFlags::NoMode)
        {
            //const char regMode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            //_BoringNumSwitch(fromReg, out, {OpCodes::movi, OpCodes::movf, OpCodes::movb});
            Serialization::SerializeInteger(OpCodes::movs, out);
            Serialization::SerializeInteger(firstReg, out);
            return secondParam;
        }

        // move from register
        if (firstReg != ModeFlags::NoMode)
        {
            Serialization::SerializeInteger(OpCodes::movr, out); 
            Serialization::SerializeInteger(firstReg, out);
            Serialization::SerializeInteger(secondReg, out);
            return Stream::Tokenize(in);
        }

        // move constant
        // constant size check
        // next = value token here
        namespace Regs = ModeFlags::RegisterModeFlags;
        const char regMode { ModeFlags::GetRegisterModeFlag(secondParam, true) };
        bool regIs8Bit { regMode >= Regs::al && regMode <= Regs::flg };

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

    std::string AddStack(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // add <mode>
        
        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        _BoringModeSwitch(mode, out, {OpCodes::addi, OpCodes::addf, OpCodes::addb});

        return Stream::Tokenize(in);
    }

    std::string AddRegister(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // addr <mode> <register> <register>
        
        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        const char reg1 { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        const char reg2 { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };

        _BoringModeSwitch(mode, out, {OpCodes::addri, OpCodes::addrf, OpCodes::addrb});
        Serialization::SerializeInteger(reg1, out);
        Serialization::SerializeInteger(reg2, out);

        return Stream::Tokenize(in);
    }
    
    std::string AddSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // adds <mode>

        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        _BoringModeSwitch(mode, out, {OpCodes::addsi, OpCodes::addsf, OpCodes::addsb});

        return Stream::Tokenize(in);
    }

    std::string HeapCopy(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // hcp
        Serialization::SerializeInteger(OpCodes::hcp, out);

        return Stream::Tokenize(in);
    }

    std::string StackCopy(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // scp
        Serialization::SerializeInteger(OpCodes::scp, out);

        return Stream::Tokenize(in);
    }

    //std::string RomCopy(AssemblyInfo& info, std::istream& in, std::ostream& out)
    //{
        //// rcp <symbol>
        //Serialization::SerializeInteger(OpCodes::rcp, out);

        //const std::string symbol { Stream::Tokenize(in) };
        //size_t symHash = String::Hash(symbol);

        //if (info.symbolMap.contains(symHash))
            //Serialization::SerializeInteger(info.symbolMap.at(symHash), out);
        //else
        //{
            //StreamPos(out, addr);
            //info.AddUnknownSymbol(symbol, addr);
            //Serialization::SerializeInteger<uint32_t>(0x00, out);
        //}

        //return Stream::Tokenize(in);
    //}

    std::string IncrementStack(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // inc <mode> <value>

        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };

        _BoringModeSwitch(mode, out, {OpCodes::inci, OpCodes::incf, OpCodes::incb}, {
            [&in, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);},
            [&in, &out](){Serialization::SerializeFloat(std::stof(Stream::Tokenize(in)), out);},
            [&in, &out](){Serialization::SerializeInteger(_TokenToInt<char>(Stream::Tokenize(in)), out);}
        });

        return Stream::Tokenize(in);
    }

    std::string IncrementRegister(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // incr <register> <value>

        const char mode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        Serialization::SerializeInteger(OpCodes::incr, out);
        Serialization::SerializeInteger(mode, out);

        namespace Regs = ModeFlags::RegisterModeFlags;
        bool regIs8Bit { mode >= Regs::al && mode <= Regs::flg };

        if (regIs8Bit)
            Serialization::SerializeInteger(_TokenToInt<char>(Stream::Tokenize(in)), out);
        else
            Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);
        
        return Stream::Tokenize(in);
    }

    std::string IncrementSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // incs <mode> <value>

        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };

        _BoringModeSwitch(mode, out, {OpCodes::incsi, OpCodes::nop, OpCodes::incsb}, {
            [&in, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);},
            [&in, &out](){Serialization::SerializeFloat(std::stof(Stream::Tokenize(in)), out);},
            [&in, &out](){Serialization::SerializeInteger(_TokenToInt<char>(Stream::Tokenize(in)), out);}
        });

        return Stream::Tokenize(in);
    }

    std::string DecrementStack(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // dcr <mode> <value>
        
        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        _BoringModeSwitch(mode, out, {OpCodes::dcri, OpCodes::dcrf, OpCodes::dcrb}, {
            [&in, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);},
            [&in, &out](){Serialization::SerializeFloat(std::stof(Stream::Tokenize(in)), out);},
            [&in, &out](){Serialization::SerializeInteger(_TokenToInt<char>(Stream::Tokenize(in)), out);}
        });

        return Stream::Tokenize(in);
    }

    std::string DecrementRegister(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // dcrr <register> <value>

        const char mode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        Serialization::SerializeInteger(OpCodes::dcrr, out);
        Serialization::SerializeInteger(mode, out);

        namespace Regs = ModeFlags::RegisterModeFlags;
        bool regIs8Bit { mode >= Regs::al && mode <= Regs::flg };

        if (regIs8Bit)
            Serialization::SerializeInteger(_TokenToInt<char>(Stream::Tokenize(in)), out);
        else
            Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);
        
        return Stream::Tokenize(in);
    }

    std::string DecrementSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // dcrs <mode> <value>

        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        _BoringModeSwitch(mode, out, {OpCodes::dcrsi, OpCodes::dcrsf, OpCodes::dcrsb}, {
            [&in, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(Stream::Tokenize(in)), out);},
            [&in, &out](){Serialization::SerializeFloat(std::stof(Stream::Tokenize(in)), out);},
            [&in, &out](){Serialization::SerializeInteger(_TokenToInt<char>(Stream::Tokenize(in)), out);}
        });
        
        return Stream::Tokenize(in);
    }

    std::string AndStack(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // and
        Serialization::SerializeInteger(OpCodes::ands, out);

        return Stream::Tokenize(in);
    }

    std::string AndRegister(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // andr <register> <register>
        Serialization::SerializeInteger(OpCodes::andr, out);

        const char reg1 { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        const char reg2 { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        Serialization::SerializeInteger(reg1, out);
        Serialization::SerializeInteger(reg2, out);

        return Stream::Tokenize(in);
    }

    std::string OrStack(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // or 
        Serialization::SerializeInteger(OpCodes::ors, out);

        return Stream::Tokenize(in);
    }

    std::string OrRegister(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // orr <register> <register>
        Serialization::SerializeInteger(OpCodes::orr, out);

        const char reg1 { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        const char reg2 { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        Serialization::SerializeInteger(reg1, out);
        Serialization::SerializeInteger(reg2, out);

        return Stream::Tokenize(in);    
    }

    std::string NorStack(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // or 
        Serialization::SerializeInteger(OpCodes::nors, out);

        return Stream::Tokenize(in);
    }

    std::string NorRegister(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // orr <register> <register>
        Serialization::SerializeInteger(OpCodes::norr, out);

        const char reg1 { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        const char reg2 { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
        Serialization::SerializeInteger(reg1, out);
        Serialization::SerializeInteger(reg2, out);

        return Stream::Tokenize(in);
    }
}
