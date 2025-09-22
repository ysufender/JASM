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
        using instructionFn = void(*)(std::istream& in);
        constexpr static  instructionFn instructions[] {
        };

        CONTEXT.PrintContext();

        const std::string& inFile { CONTEXT.InputFiles()[0] };
        std::ifstream source { System::OpenInFile(inFile) };

        AssemblyInfo info { "", 0 }; 

        source.seekg(-sizeof(uint64_t), std::ios::end);
        uint64_t size { };
        Serialization::DeserializeInteger(size, source);
        source.seekg(-(sizeof(uint64_t)+size), std::ios::end);
        IStreamPos(source, bytecodeEnd);
        info.Deserialize(source);

        info.PrintAssemblyInfo();

        source.seekg(0, std::ios::beg);
        if (info.flags & AssemblyFlags::Executable)
        {
            uint32_t dat;
            Serialization::DeserializeInteger(dat, source);
            std::cout << "\norg " << dat;
            Serialization::DeserializeInteger(dat, source);
            std::cout << "\nsts " << dat;
            Serialization::DeserializeInteger(dat, source);
            std::cout << "\nsth " << dat;
        }

        while (source.tellg() < bytecodeEnd)
        {
            char dat { };
            source.read(&dat, 1);
            std::cout << static_cast<uint32_t>(dat) << (source.tellg() % 9 == 8 ? '\n': ' ');
        }
        std::cout << '\n';
    }
}
