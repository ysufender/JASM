#include "test.hpp"

#include "JASMConfig.hpp"

#include "extensions/system.hpp"
#include "assemblycontext.hpp"

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
