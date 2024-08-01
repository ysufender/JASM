#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "JASMConfig.hpp"
#include "assemblycontext.hpp"

AssemblyContext::AssemblyContext(
        bool silent,
        bool single, 
        const std::string& out, 
        const std::string& libT, 
        const std::vector<std::string>& in,
        const std::vector<std::string>& libs
    ) : silentMode(silent), singleAssembly(single)
{
    if (in.size() == 0)
    {
        throw std::runtime_error { "At least one input file is needed." };
        return;
    }

    inputFiles = in;
    isLib = (libT == "shd" || libT == "stc");
    libType = (isLib ? (libT == "shd" ? LibTypeEnum::Shared : LibTypeEnum::Static) : LibTypeEnum::Executable);
    libraries = libs;

    std::stringstream ss;

    // Input file extensions are '.jasm' for files
    if (out.empty())
    {
        std::string copy { inputFiles.at(0) };
        ss << copy.erase(copy.size()-5, 5);
    }
    else
        ss << out;

    ss << "." << (isLib ? libT : "jef");
    outFile = ss.str();
}

const bool& AssemblyContext::IsSilent() const { return silentMode; }
const bool& AssemblyContext::IsSingle() const { return singleAssembly; }
const std::string& AssemblyContext::OutFile() const { return outFile; }
const bool& AssemblyContext::IsLib() const { return isLib; }
LibTypeEnum AssemblyContext::LibType() const { return libType; }
const std::vector<std::string>& AssemblyContext::InputFiles() const { return inputFiles; }
const std::vector<std::string>& AssemblyContext::Libraries() const { return libraries; }

void AssemblyContext::PrintContext() const 
{
    if (silentMode)
        return;

    std::cout << "JASM Version " << JASM_VERSION << " Assembly Context"
              << "\n\tSingle Mode: " << (singleAssembly ? "Enabled" : "Disabled");

    if (!singleAssembly) { std::cout << "\n\tOutput File: " << outFile; }
    if (isLib) { std::cout << "\n\tLibrary Type: " << (libType == LibTypeEnum::Shared ? "Shared" : "Static" ); }

    std::cout << "\n\tInput Files: " << inputFiles.size() << " total {";
    for (const std::string& file : inputFiles)
    {
        std::cout << "\n\t\t\"" << file << '\"';
        
        if (inputFiles.back() != file)
            std::cout << ',';
    }
    std::cout << "\n\t}";
    
    if(libraries.size() == 0)
    {
        std::cout << "\n\tLibraries: No Library Provided\n";
        return;
    }

    std::cout << "\n\tLibraries: " << libraries.size() << " total {";
    for (const std::string& lib : libraries) 
    {
        std::cout << "\n\t\t\"" << lib << '\"';

        if (libraries.back() != lib)
            std::cout << ',';
    }
    std::cout << "\n\t}\n";
}


//
// Tests
//
#include "test/test.hpp"

#ifdef TEST_MODE
#include "extensions/system.hpp"

TEST_CASE("Assembly Context Tests")
{
    SECTION("AssemblyContext::ctor::Error")
    {
        CHECK_THROWS_WITH(
            AssemblyContext (false, false, "out", "stc", {}, {}),
            "At least one input file is needed."
        );
    }

    SECTION("AssemblyContext::Correctness_Check")
    {
        SECTION("AssemblyContext::Correctness_Check::Static")
        {
            AssemblyContext context { false, false, "out", "stc", {"testOut.jasm"}, {} };
            CHECK(context.IsLib());
            CHECK(context.LibType() == LibTypeEnum::Static);
            CHECK(context.OutFile() == "out.stc");
        }

        SECTION("AssemblyContext::Correctness_Check::Shared")
        {
            AssemblyContext context { false, false, "out", "shd", {"testOut.jasm"}, {} };
            CHECK(context.IsLib());
            CHECK(context.LibType() == LibTypeEnum::Shared);
            CHECK(context.OutFile() == "out.shd");
        }

        SECTION("AssemblyContext::Correctness_Check::General")
        {
            AssemblyContext context { false, true, "outTest", "", {"test.jasm", "a.jasm"}, {"lib.stc"}};

            CHECK(context.LibType() == LibTypeEnum::Executable);
            CHECK(context.OutFile() == "outTest.jef");
            CHECK_FALSE(context.IsLib());
            CHECK_FALSE(context.IsSilent());
            CHECK(context.IsSingle());
            CHECK(context.Libraries().size() == 1);
            CHECK(context.InputFiles().size() == 2);

            std::stringstream ss { };
            std::stringstream fakeCout;

            System::Setup(context, ss, ss);
            context.PrintContext();

            fakeCout << "JASM Version " << JASM_VERSION << " Assembly Context"
                << "\n\tSingle Mode: " << (context.IsSingle() ? "Enabled" : "Disabled");

            if (!context.IsSingle()) { fakeCout << "\n\tOutput File: " << context.OutFile(); }
            if (context.IsLib()) { fakeCout << "\n\tLibrary Type: " << (context.LibType() == LibTypeEnum::Shared ? "Shared" : "Static" ); }

            fakeCout << "\n\tInput Files: " << context.InputFiles().size() << " total {";
            for (const std::string& file : context.InputFiles())
            {
                fakeCout << "\n\t\t\"" << file << '\"';

                if (context.InputFiles().back() != file)
                    fakeCout << ',';
            }
            fakeCout << "\n\t}";

            if(context.Libraries().size() == 0)
            {
                fakeCout << "\n\tLibraries: No Library Provided\n";
                return;
            }

            fakeCout << "\n\tLibraries: " << context.Libraries().size() << " total {";
            for (const std::string& lib : context.Libraries()) 
            {
                fakeCout << "\n\t\t\"" << lib << '\"';

                if (context.Libraries().back() != lib)
                    fakeCout << ',';
            }
            fakeCout << "\n\t}\n";

            CHECK(ss.str() == fakeCout.str());
        }
    }
}
#endif
