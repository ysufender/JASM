#pragma once

#include <string>
#include <vector>

struct AssemblyContext 
{
    private:
        bool singleAssembly;
        bool isLib;
        std::string outFile;
        std::string libType;
        std::vector<std::string> inputFiles;
        std::vector<std::string> libraries;

    public:
        AssemblyContext(
            bool single, 
            std::string out, 
            std::string libT, 
            std::vector<std::string> in,
            std::vector<std::string> libs
        );

        const bool& IsSingle() const;
        const bool& IsLib() const;
        const std::string& OutFile() const;
        const std::string& LibType() const;
        const std::vector<std::string>& InputFiles() const;
        const std::vector<std::string>& Libraries() const;

        void PrintContext() const;
};