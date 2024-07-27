#include <array>
#include <cassert>
#include <concepts>
#include <ios>
#include <ostream>
#include <string>
#include <iostream>
#include <type_traits>

#include "assembler/instructions.hpp"
#include "assembler/modeflags.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "extensions/stringextensions.hpp"
#include "extensions/system.hpp"


namespace Instructions
{
    //
    // Utils
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
    constexpr auto dummyCallback = [](){};
    void _BoringNumSwitch(const char modeFlag, std::ostream& out, std::array<char, 3> op, std::array<std::function<void()>, 3> callbacks = {dummyCallback, dummyCallback, dummyCallback})
    {
        namespace NModes = ModeFlags::NumericModeFlags;

        switch (modeFlag)
        {
            case NModes::UInt: 
            case NModes::Int: 
                Serialization::SerializeInteger(op.at(0), out);
                callbacks.at(0)();
                break;

            case NModes::Float:
                Serialization::SerializeInteger(op.at(1), out);
                callbacks.at(1)();
                break;

            case NModes::UByte:
            case NModes::Byte:
                Serialization::SerializeInteger(op.at(2), out); 
                callbacks.at(2)();
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
        // stc <float>

        std::string next { Stream::Tokenize(in) };
        const char modeFlag { ModeFlags::GetModeFlag(next) };

        if (modeFlag != ModeFlags::NoMode)
        {
            const std::string numToken = Stream::Tokenize(in);

            static const auto intCallback   = [&numToken, &out](){Serialization::SerializeInteger(_TokenToInt<uint32_t>(numToken), out);};
            static const auto floatCallback = [&numToken, &out](){Serialization::SerializeFloat(std::stof(numToken), out);};
            static const auto byteCallback  = [&numToken, &out](){Serialization::SerializeInteger(_TokenToInt<char>(numToken), out);};
            
            _BoringNumSwitch(modeFlag, out, {OpCodes::sti, OpCodes::stf, OpCodes::stb}, {
                [&numToken, &out](){Serialization::SerializeInteger(_TokenToInt<uint32_t>(numToken), out);},
                [&numToken, &out](){Serialization::SerializeFloat(std::stof(numToken), out);},
                [&numToken, &out](){Serialization::SerializeInteger(_TokenToInt<char>(numToken), out);},
            });
        }
        else
        {
            if (next.find_first_of('.') == std::string::npos)
                LOGE(System::LogLevel::High, "Expected mode flag after '", Stream::Tokenize(in, true), "'");

            Serialization::SerializeInteger(OpCodes::stf, out);
            Serialization::SerializeFloat(std::stof(next), out);
        }
    }

    void LoadConstant(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // ldc <mode> <value>
        // ldc <float>

        const std::string next { Stream::Tokenize(in) };
        const char modeFlag { ModeFlags::GetModeFlag(next) };

        if (modeFlag != ModeFlags::NoMode)
        {
            const std::string numToken = Stream::Tokenize(in);

            static const auto intCallback   = [&numToken, &out](){Serialization::SerializeInteger(_TokenToInt<uint32_t>(numToken), out);};
            static const auto floatCallback = [&numToken, &out](){Serialization::SerializeFloat(std::stof(numToken), out);};
            static const auto byteCallback  = [&numToken, &out](){Serialization::SerializeInteger(_TokenToInt<char>(numToken), out);};

            _BoringNumSwitch(modeFlag, out, {OpCodes::ldi, OpCodes::ldf, OpCodes::ldb}, {
                intCallback,
                floatCallback,
                byteCallback
            });
        }
        else
        {
            if (next.find_first_of('.') == std::string::npos)
                LOGE(System::LogLevel::High, "Expected mode flag after '", Stream::Tokenize(in, true), "'");

            Serialization::SerializeInteger(OpCodes::ldf, out);
            Serialization::SerializeFloat(std::stof(next), out);
        }
    }

    void ReadAddress(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // rda <register>
        // rda <mode> <register>

        const std::string next { Stream::Tokenize(in) };

        if (next.starts_with('&')) // register
        {
            const char regMode { ModeFlags::GetRegisterModeFlag(next) };
            Serialization::SerializeInteger(OpCodes::rdr, out);
            Serialization::SerializeInteger(regMode, out);
        }
        else if (next.starts_with('%'))
        {
            const char numMode { ModeFlags::GetModeFlag(next) };
            const char regMode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in)) };

            _BoringNumSwitch(numMode, out, {OpCodes::rdi, OpCodes::rdf, OpCodes::rdb});

            Serialization::SerializeInteger(regMode, out);
        }
    }

    void Move(AssemblyInfo& info, std::istream& in, std::ostream& out)
    {
        // mov <register> <register>
        // mov <mode> <value> <register>
        // mov <value> <register>

        const std::string next { Stream::Tokenize(in) };

        if (next.starts_with('&'))
        {
            const std::string regToken { Stream::Tokenize(in) };

            const char reg1 { ModeFlags::GetRegisterModeFlag(next) };
            const char reg2 { ModeFlags::GetRegisterModeFlag(regToken) };

            Serialization::SerializeInteger(OpCodes::movr, out);
            Serialization::SerializeInteger(reg1, out);
            Serialization::SerializeInteger(reg2, out);
        }
        else if (next.starts_with('%'))
        {
            const char numMode { ModeFlags::GetModeFlag(next) };
            const std::string numToken { Stream::Tokenize(in) };
            const char regMode { ModeFlags::GetRegisterModeFlag(Stream::Tokenize(in)) }; 
            
            static const auto intCallback   = [&numToken, &out](){Serialization::SerializeInteger(_TokenToInt<uint32_t>(numToken), out);};
            static const auto floatCallback = [&numToken, &out](){Serialization::SerializeFloat(std::stof(numToken), out);};
            static const auto byteCallback  = [&numToken, &out](){Serialization::SerializeInteger(_TokenToInt<char>(numToken), out);};

            _BoringNumSwitch(numMode, out, {OpCodes::movi, OpCodes::movf, OpCodes::movb}, {
                intCallback,
                floatCallback,
                byteCallback
            });

            Serialization::SerializeInteger(regMode, out);
        }
        else
        {
            if (next.find_first_of('.') == std::string::npos)
                LOGE(System::LogLevel::High, "Expected mode flag after '", Stream::Tokenize(in, true), "'");

            Serialization::SerializeInteger(OpCodes::movf, out);
            Serialization::SerializeFloat(std::stof(next), out);
        }
    }
}