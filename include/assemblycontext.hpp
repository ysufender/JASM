#pragma once

#include <string>
#include <vector>
#include <ostream>

class AssemblyContext 
{
    private:
        bool silentMode;
        bool singleAssembly;
        bool isLib;
        std::string outFile;
        std::string libType;
        std::vector<std::string> inputFiles;
        std::vector<std::string> libraries;

    public:
        AssemblyContext(
            bool silent,
            bool single, 
            const std::string& out, 
            const std::string& libT, 
            const std::vector<std::string>& in,
            const std::vector<std::string>& libs
        );

        const bool& IsSilent() const;
        const bool& IsSingle() const;
        const bool& IsLib() const;
        const std::string& OutFile() const;
        const std::string& LibType() const;
        const std::vector<std::string>& InputFiles() const;
        const std::vector<std::string>& Libraries() const;

        void PrintContext(std::ostream& out) const;
};