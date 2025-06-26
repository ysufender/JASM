#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

#include "CLIParser.hpp"

#include "jasm.hpp"
#include "bytemode/linker/linker.hpp"
#include "system.hpp"
#include "JASMConfig.hpp"
#include "bytemode/assembler/assembler.hpp"

int jasmmain(int argc, char** args)
{
    try
    {
        CLIParser::Flags flags { SetUpCLI(args, argc) };

        if (argc == 1 || flags.GetFlag<CLIParser::FlagType::Bool>("help"))
            PrintHelp(flags);
        else if (flags.GetFlag<CLIParser::FlagType::Bool>("version"))
            LOG("JASM Version ", JASM_VERSION);
        else
        {
            //SetStdout(flags);
            System::Setup(flags, std::cout, std::cerr);

            using BAsm = typename ByteAssembler::ByteAssembler;
            using BLink = typename ByteLinker::ByteLinker;

            BAsm assembler;
            ByteAssembler::AssemblyInfoCollection collection { assembler.Assemble() };

            BLink linker;
            linker.Link(collection);
        }
    }
    catch (const JASMException& exc)
    {
        std::cerr << "ERROR: " << exc << std::endl;
        return 1;
    }
    catch (const std::exception& exc)
    {
        std::cerr << "An unexpected error occured during process.\n\tProvided information: " 
                  << exc.what() << std::endl;
        return 1;
    }

    return 0;
}

void PrintHeader() noexcept
{
    std::cout << "\nJust an Assembler (JASM)"
              << "\n\tDescription: " << JASM_DESCRIPTION
              << "\n\tVersion: " << JASM_VERSION
              << "\n\tUse Pipelines: "
#ifdef USE_PIPELINES_OPT
              << "Available"
#else
              << "Unavailable"
#endif
              << "\n\tBuild Mode: "
#ifdef TOOLCHAIN_MODE
              << "Toolchain"
#else
              << "CLI"
#endif
              << '\n';
}

void PrintHelp(const CLIParser::Flags& flags) noexcept
{
    PrintHeader();
    std::cout << flags.GetHelpText()
              << "\n\n\n\t" << "WARNING: In single mode, each file will be assembled as a static library. Otherwise the output will be decided by `--libType` flag.\n\n";
}

void SetStdout(const CLIParser::Flags& flags)
{
    std::vector<std::string> rdout { flags.GetFlag<CLIParser::FlagType::StringList>("redirect-stdout") };

    std::ofstream out;
    std::ofstream err;

    if (!rdout.empty())
    {
        if (rdout.size() == 0)
            LOGW("At least one path must be given after flag `--redirect-output (-r)`. Using default settings.");
        else if (rdout.size() == 1)
        {
            out = System::OpenOutFile(rdout[0]);
            err = System::OpenOutFile(rdout[0]);
        }
        else
        {
            out = System::OpenOutFile(rdout[0]);
            err = System::OpenOutFile(rdout[1]);
        }

        if (rdout.size() > 2)
            LOGE(System::LogLevel::Low, "--redirect-stdout cannot take more than 2 arguments.");
    }

    if (out.is_open() && err.is_open())
        System::Setup(flags, out, err);
    else
        System::Setup(flags, std::cout, std::cerr);
}

CLIParser::Flags SetUpCLI(char** args, int argc)
{
    using namespace CLIParser;

    Parser parser{args, argc, "--", "-"};
    parser.AddFlag<FlagType::Bool>("help", "Print this help text.");
    parser.AddFlag<FlagType::Bool>("version", "Print version.");
    parser.Separator();
    parser.AddFlag<FlagType::Bool>("silent", "Disables SOME outputs. So it's less painful to look at the screen.", false);
    parser.AddFlag<FlagType::Bool>("single", "Assemble each file provided by '--in' flag but do NOT link.", false);
    parser.AddFlag<FlagType::String>("out", "Place the output file to specified path if flag '--single' is not set.");
    parser.AddFlag<FlagType::String>("lib-type", "If desired output is a library, specify the type. (either shared 'shd' or static 'stc')");
    parser.Separator();
    parser.AddFlag<FlagType::StringList>("in", "Files to assemble and (optionally) link. The first entry must contain the entry point, if output is executable.");
    parser.AddFlag<FlagType::StringList>("libs", "Libraries used and to be linked.");
    parser.Separator();
    parser.AddFlag<FlagType::Bool>("pipelines", "Use pipeline-streams instead of intermediate files.", false);
    parser.AddFlag<FlagType::String>("working-dir", "Set the working directory of the assembler.", std::filesystem::current_path().generic_string());
    parser.AddFlag<FlagType::StringList>("redirect-stdout", "Redirect stdout and stderr to given files. If only one is provided, both get redirected to it.");

    parser.BindFlag("h", "help");
    parser.BindFlag("v", "version");
    parser.BindFlag("s", "silent");
    parser.BindFlag("S", "single");
    parser.BindFlag("o", "out");
    parser.BindFlag("I", "in");
    parser.BindFlag("L", "libs");
    parser.BindFlag("p", "pipelines");
    parser.BindFlag("w", "working-dir");
    parser.BindFlag("r", "redirect-stdout");

    return parser.Parse();
}
