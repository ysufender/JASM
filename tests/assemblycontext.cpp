#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"

#include "JASMConfig.hpp"

#include "system.hpp"
#include "assemblycontext.hpp"
#include <filesystem>

TEST_CASE("Assembly Context Tests")
{
    SECTION("AssemblyContext::ctor::Error")
    {
        CHECK_THROWS_AS(
            AssemblyContext (false, false, false, "out", "stc", ".", { }, { }),
            JASMException
        );
    }

    SECTION("AssemblyContext::Correctness_Check")
    {
        SECTION("AssemblyContext::Correctness_Check::Static")
        {
            AssemblyContext context { false, false, false, "out", "stc", "", {"testOut.jasm"}, {} };
            CHECK(context.IsLib());
            CHECK(context.LibType() == LibTypeEnum::Static);
            CHECK(context.OutFile() == "out.stc");
        }

        SECTION("AssemblyContext::Correctness_Check::Shared")
        {
            AssemblyContext context { false, false, false, "out", "shd", "", {"testOut.jasm"}, {} };
            CHECK(context.IsLib());
            CHECK(context.LibType() == LibTypeEnum::Shared);
            CHECK(context.OutFile() == "out.shd");
        }

        SECTION("AssemblyContext::Correctness_Check::General")
        {
            AssemblyContext context { false, true, false, "outTest", "", "", {"test.jasm", "a.jasm"}, {"lib.stc"}};

            CHECK(context.LibType() == LibTypeEnum::Executable);
            CHECK(context.OutFile() == "outTest.jef");
            CHECK_FALSE(context.IsUsingPipelines());
            CHECK_FALSE(context.IsLib());
            CHECK_FALSE(context.IsSilent());
            CHECK(context.IsSingle());
            CHECK(context.WorkingDir() == std::filesystem::current_path().generic_string());
            CHECK(context.Libraries().size() == 1);
            CHECK(context.InputFiles().size() == 2);

            std::stringstream ss;
            std::stringstream fakeCout;

            System::Setup(context, ss, ss);
            context.PrintContext();
            System::Setup(context, fakeCout, fakeCout);
            context.PrintContext();

            CHECK(ss.view() == fakeCout.view());
        }
    }
}
