#include <array>
#include <cassert>
#include <cctype>
#include <concepts>
#include <ostream>
#include <string>
#include <iostream>

#include "bytemode/assembler/instructions.hpp"
#include "JASMConfig.hpp"
#include "bytemode/assembler/modeflags.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "extensions/stringextensions.hpp"
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
        if (token.starts_with("0x"))
            return String::HexToInt<T>(token);
        else
            return static_cast<T>(std::stoul(token));
    }


    // I don't care about anything, I just don't want to write this
    // thing again and again
    void _BoringNumSwitch(const char modeFlag, std::ostream& out, std::array<char, 3> op, std::array<std::function<void()>, 3> callbacks = {})
    {
        namespace NModes = ModeFlags::NumericModeFlags;

        static const auto callTheBack = [&callbacks](int index){
            if (callbacks.at(index))
                callbacks.at(index)();
        };

        switch (modeFlag)
        {
            case NModes::UInt: 
            case NModes::Int: 
                Serialization::SerializeInteger(op.at(0), out);
                callTheBack(0);
                break;

            case NModes::Float:
                Serialization::SerializeInteger(op.at(1), out);
                callTheBack(1);
                break;

            case NModes::UByte:
            case NModes::Byte:
                Serialization::SerializeInteger(op.at(2), out); 
                callTheBack(2);
                break;

            default:
                LOGE(System::LogLevel::High, "Unexpected mode flag '", std::to_string(modeFlag), "'");
                break;
        }
    }

    //
    // Implementation
    //
    void StoreConstant(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // stc <mode> <value>
        // stc <mode> <symbol>
        // stc <float>
        // stc <integer>

        const std::string next { Stream::Tokenize(in) };
        const char modeFlag { ModeFlags::GetModeFlag(next) };

        // Store integer or float
        if (modeFlag == ModeFlags::NoMode)
        {
            namespace NMode = ModeFlags::NumericModeFlags;

            const char fakeFlag { next.find_first_of('.' == std::string::npos) ? NMode::Int : NMode::Float };

            _BoringNumSwitch(fakeFlag, out, {OpCodes::sti, OpCodes::stf, 0x00}, {
                [&next, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(next), out);},
                [&next, &out](){Serialization::SerializeFloat(std::stof(next), out);}
            });

            return;
        }

        const std::string symOrVal { Stream::Tokenize(in) };

        // Store constant by mode
        // stc $<i/f/b> <value>
        if (std::isdigit(symOrVal.at(0)))
        {
            _BoringNumSwitch(modeFlag, out, {OpCodes::sti, OpCodes::stf, OpCodes::stb}, {
                [&symOrVal, &out](){Serialization::SerializeInteger(_TokenToInt<systembit_t>(symOrVal), out); },
                [&symOrVal, &out](){Serialization::SerializeFloat(std::stof(symOrVal), out); },
                [&symOrVal, &out](){Serialization::SerializeInteger(_TokenToInt<char>(symOrVal), out); },
            });
            return;
        }
            

        // Store symbol by mode
        _BoringNumSwitch(modeFlag, out, {OpCodes::stis, OpCodes::stfs, OpCodes::stbs});

        size_t symbolHash { String::Hash(symOrVal) };

        if (!info.symbolMap.contains(symbolHash)) 
        {
            info.unknownSymbols.push_back({symbolHash, static_cast<systembit_t>(out.tellp())});
            Serialization::SerializeInteger<systembit_t>(0, out);
        }
        else 
            Serialization::SerializeInteger(info.symbolMap.at(symbolHash), out);
    }

    void LoadConstant(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // ldc <mode>
        const std::string next { Stream::Tokenize(in) };
        const char modeFlag { ModeFlags::GetModeFlag(next) };

        _BoringNumSwitch(modeFlag, out, {OpCodes::ldi, OpCodes::ldf, OpCodes::ldb}, { });
    }

    void ReadAddress(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // rda <register>
        // rda <mode>

        const std::string next { Stream::Tokenize(in) };
        char modeFlag { ModeFlags::GetRegisterModeFlag(next) };

        if (modeFlag != ModeFlags::NoMode) // register
        {
            const char regMode { ModeFlags::GetRegisterModeFlag(next) };
            Serialization::SerializeInteger(OpCodes::rdr, out);

            return;
        }
        
        modeFlag = ModeFlags::GetModeFlag(next);
        const char regMode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };

        _BoringNumSwitch(modeFlag, out, {OpCodes::rdi, OpCodes::rdf, OpCodes::rdb});

        Serialization::SerializeInteger(regMode, out);
    }

    void Move(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // mov <register> [from stack]
        // mov <register> <register> [from register]
        // mov <value> <register> [from constant]

        const std::string next { Stream::Tokenize(in) };
        char modeFlag { ModeFlags::GetModeFlag(next) };

        // move from stack
        if (modeFlag != ModeFlags::NoMode)
        {
            const char regMode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in), true) };
            _BoringNumSwitch(modeFlag, out, {OpCodes::movi, OpCodes::movf, OpCodes::movb});
            Serialization::SerializeInteger(regMode, out);

            return;
        }


        // move from register
        modeFlag = ModeFlags::GetRegisterModeFlag(next);;
        if (modeFlag != ModeFlags::NoMode)
        {
            char fromReg { modeFlag };
            const std::string regToken { Stream::Tokenize(in) };
            const char toReg { ModeFlags::GetRegisterModeFlag(regToken, true) };; 

            Serialization::SerializeInteger(OpCodes::movr, out); 
            Serialization::SerializeInteger(fromReg, out);
            Serialization::SerializeInteger(toReg, out);
            return;
        }

        // move from symbol
        //if (!std::isdigit(next.at(0)))
        //{
            // next = symbolName here
            //const std::string regToken { Stream::Tokenize(in) };
            //onst char regMode { ModeFlags::GetRegisterModeFlag(regToken, true) };
            //systembit_t addr { info.symbolMap.contains(next) ? info.symbolMap.at(next) : 0 };
            
            //Serialization::SerializeInteger(OpCodes::movs, out);

            //if (!info.symbolMap.contains(next))
                //info.unknownSymbols.push_back({next, static_cast<systembit_t>(out.tellp())});

            //Serialization::SerializeInteger(addr, out);
            //Serialization::SerializeInteger(regMode, out);
            //return;
        //}


        // move constant
        // constant size check
        // next = value token here
        namespace Regs = ModeFlags::RegisterModeFlags;
        const std::string regToken { Stream::Tokenize(in) };
        const char regMode { ModeFlags::GetRegisterModeFlag(regToken, true) };
        bool regIs8Bit { regMode >= Regs::al && regMode <= Regs::FLG };

        Serialization::SerializeInteger(OpCodes::movc, out);

        if (next.find_first_of('.') == std::string::npos)
        {
            if (regIs8Bit)
                Serialization::SerializeInteger(_TokenToInt<char>(next), out); 
            else
                Serialization::SerializeInteger(_TokenToInt<systembit_t>(next), out); 
        }
        else if (!regIs8Bit)
            Serialization::SerializeFloat(std::stof(next), out);
        else
            LOGE(System::LogLevel::High, "Register '", regToken, "' is too small for float value '", next, "'");

        Serialization::SerializeInteger(regMode, out);
    }

    void Add(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // add <mode>
        
        const char mode { ModeFlags::GetModeFlag(Stream::Tokenize(in), true) };
        _BoringNumSwitch(mode, out, {OpCodes::addi, OpCodes::addf, OpCodes::addb});
    }

    void AddSafe(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
    }
}
