#include <filesystem>
#include <sstream>
#include <fstream>

#include "jasm.hpp"
#include "CLIParser.hpp"
#include "JASMConfig.hpp"

#ifndef TEST_MODE
#include "extensions/system.hpp"
#include "assembler/assembler.hpp"
#include "assemblycontext.hpp"

int main(int argc, char** args)
{
    using namespace CLIParser;

    try
    {
        //"Available Flags:
        //--help, -h: Print help.
        //--version, -v: Print version.
        //
        //--silent, -s : Disables SOME outputs. So it's less painful to look at the screen.
        //--single, -S: Assemble each file provided by `-in` but do NOT link.
        //--out, -o: Place the output file to specified path if flag `-single` is not set.
        //--libType <lib_type>: If desired output is a library, specify the type. (either shared 'shd' or static 'stc')
        //
        //--in <..input..>, -I <..input..>: Files to assemble and (optionally) link. The first entry is treated as jasm file containing entry point.
        //--libs <..libs..>, -L <..libs..>: Libraries used and to be linked.
        //
        //--pipelines, -p: Use pipeline-streams instead of intermediate files.
        //--working-dir, -w: Set the working directory of the assembler.
        //--redirect-stdout <..files..>, -r <..files..>: Redirect stdout and stderr to given files, respectively. If only one file is given, both get
        //                                               redirected to it.
        //
        //WARNING:
        //    In single mode, each file will be assembled as a static library. Otherwise the output will be decided by `--libType` flag.
        //)"
        
        Parser parser{args, argc, "--", "-"};
        parser.AddFlag<FlagType::Bool>("help", "Print this help text.", true);
        parser.AddFlag<FlagType::Bool>("version", "Print version.", false);
        parser.Seperator();
        parser.AddFlag<FlagType::Bool>("silent", "Disables SOME outputs. So it's less painful to look at the screen.", false);
        parser.AddFlag<FlagType::Bool>("single", "Assemble each file provided by '--in' flag but do NOT link.", false);
        parser.AddFlag<FlagType::String>("out", "Place the output file to specified path if flag '--single' is not set.");
        parser.AddFlag<FlagType::String>("lib-type", "If desired output is a library, specify the type. (either shared 'shd' or static 'stc')");
        parser.Seperator();
        parser.AddFlag<FlagType::StringList>("in", "Files to assemble and (optionally) link. The first entry must contain the entry point, if output is executable.");
        parser.AddFlag<FlagType::StringList>("libs", "Libraries used and to be linked.");
        parser.Seperator();
        parser.AddFlag<FlagType::Bool>("pipelines", "Use pipeline-streams instead of intermediate files.", false);
        parser.AddFlag<FlagType::String>("working-dir", "Set the working directory of the assembler.", std::filesystem::current_path());
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

        Flags flags = parser.Parse();

        if (flags.GetFlag<FlagType::Bool>("help"))
            PrintHelp(flags);
        else if (flags.GetFlag<FlagType::Bool>("version"))
            std::cout << "JASM Version " << JASM_VERSION << '\n';
        else
        {
            std::vector<std::string> rdout { flags.GetFlag<FlagType::StringList>("redirect-stdout") };

            if (!rdout.empty())
            {
                if (rdout.size() == 0)
                    LOGW("At least one path must be given after flag `--redirect-output (-r)`. Using default settings.");
                else if (rdout.size() == 1)
                {
                    std::ofstream file { rdout[0] };
                    System::Setup(flags, file, file);
                }
            }


            Assembler assembler;
            Assembler::AssemblyInfoCollection collection = assembler.Assemble();
        }
    }
    catch (const JASMException& exc)
    {
        std::cerr << "An error occured during process."
                  << "\n\tProvided information: " << exc << std::endl;

        return 1;
    }

    return 0;
}
#endif

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
    std::cout << flags.GetHelpText() << '\n'
              << "\n\n\t" << "WARNING: In single mode, each file will be assembled as a static library. Otherwise the output will be decided by `--libType` flag.\n";

}
