#include <cassert>
#include <ios>
#include <string>
#include <iostream>

#include "assembler/instructions.hpp"
#include "extensions/serialization.hpp"
#include "extensions/streamextensions.hpp"
#include "extensions/stringextensions.hpp"
#include "extensions/system.hpp"


namespace Instructions
{
    using namespace Extensions;

    void Nop(AssemblyInfo& info, std::ifstream& in, std::ofstream& out)
    {
        Serialization::SerializeInteger(OpCodes::nop, out);
    }

    void StoreInt(AssemblyInfo& info, std::ifstream& in, std::ofstream& out)
    {
        // sti <integer>
        int32_t integer;

        std::string integerToken { Stream::Tokenize(in) };
        if (integerToken.starts_with("0x"))
            integer = String::HexToInt<int32_t>(integerToken);
        else
            integer = std::stoi(integerToken);

        Serialization::SerializeInteger(OpCodes::sti, out);
        Serialization::SerializeInteger(integer, out);
    }

    void StoreFloat(AssemblyInfo& info, std::ifstream& in, std::ofstream& out)
    {
        // stf <float>
        float floatVal { std::stof(Stream::Tokenize(in)) };
        Serialization::SerializeInteger(OpCodes::stf, out);
        Serialization::SerializeFloat(floatVal, out);
    }

    void StoreByte(AssemblyInfo& info, std::ifstream& in, std::ofstream& out)
    {
        // stb <byte>
        char byte { static_cast<char>(std::stoi(Stream::Tokenize(in))) };
        Serialization::SerializeInteger(OpCodes::stb, out);
        Serialization::SerializeInteger(byte, out);
    }

    void StoreUInt(AssemblyInfo& info, std::ifstream& in, std::ofstream& out)
    {
        // stui <uint>
    }

    void StoreUByte(AssemblyInfo& info, std::ifstream& in, std::ofstream& out)
    {
        // stub <ubyte>
    }
}