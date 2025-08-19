#pragma once

#include <filesystem>
#include <string>
#include <string_view>
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
        bool _symbolInfo;
        bool _storeName;
        std::string _outFile;
        std::filesystem::path _workingDir;
        LibTypeEnum _libType;
        std::vector<std::string> _inputFiles;
        std::vector<std::string> _libraries;

        // I know it's a bad practice but...
        mutable std::string _contextString;

    public:
        AssemblyContext(
            bool silent,
            bool single, 
            bool pipelines,
            const std::string& out, 
            const std::string& libT, 
            const std::filesystem::path& workingDir,
            const std::vector<std::string>& in,
            const std::vector<std::string>& libs,
            bool storeSymbols,
            bool storeName
        );

        AssemblyContext() = delete;
        //AssemblyContext(AssemblyContext&&) = delete;
        AssemblyContext(AssemblyContext&) = delete;


        bool IsSilent() const;
        bool IsSingle() const;
        bool IsUsingPipelines() const;
        bool IsLib() const;
        bool StoreSymbols() const;
        bool StoreName() const;
        std::string_view OutFile() const;
        LibTypeEnum LibType() const;
        const std::filesystem::path& WorkingDir() const;
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
    /*.libraries =*/ { },
    /*.symbolInfo =*/ false,
    /*.storeName =*/ false
};
