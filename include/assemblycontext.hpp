#pragma once

#include <bitset>
#include <string>
#include <vector>

enum class LibTypeEnum 
{
    Static,
    Shared,
    Executable
};

class AssemblyContext 
{
    private:
        bool _silentMode;
        bool _singleAssembly;
        bool _isLib;
        bool _pipelines;
        std::string _outFile;
        std::string _workingDir;
        LibTypeEnum _libType;
        std::vector<std::string> _inputFiles;
        std::vector<std::string> _libraries;

        mutable std::string _contextString;

    public:
        // silent, single, out, libType, in, libs
        AssemblyContext(
            bool silent,
            bool single, 
            bool pipelines,
            const std::string& out, 
            const std::string& libT, 
            const std::string& workingDir,
            const std::vector<std::string>& in,
            const std::vector<std::string>& libs
        );

        AssemblyContext() = delete;
        //AssemblyContext(AssemblyContext&&) = delete;
        AssemblyContext(AssemblyContext&) = delete;


        const bool& IsSilent() const;
        const bool& IsSingle() const;
        const bool& IsLib() const;
        const std::string& OutFile() const;
        LibTypeEnum LibType() const;
        const std::vector<std::string>& InputFiles() const;
        const std::vector<std::string>& Libraries() const;

        void PrintContext() const;
};

static AssemblyContext DefaultContext { 
    /*.silentMode =*/ false, 
    /*.singleAssembly =*/ false, 
    /*.pipelines =*/ false,
    /*.outFile =*/ "", 
    /*.workingDir =*/ "",
    /*.libType =*/ "", 
    /*.inputFiles =*/ { "none.jasm" }, 
    /*.libraries =*/ { }
};
