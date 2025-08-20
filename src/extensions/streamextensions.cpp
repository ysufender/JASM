#include <cctype>
#include <cstdlib>
#include <istream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>

#include "JASMConfig.hpp"
#include "extensions/streamextensions.hpp"
#include "system.hpp"

namespace Extensions::Stream
{
    std::unordered_map<std::string, std::string> macros { };

    std::string Tokenize(std::istream& inputStream)
    {
        char ch;
        std::stringstream ss;

        inputStream.read(&ch, sizeof(ch));

        while (true)
        {
            if (inputStream.bad())
                LOGE(System::LogLevel::High, "An error occured while reading the source file.");

            if (inputStream.eof())
                return JASM_EOF;

            while (isspace(ch))
                inputStream.read(&ch, sizeof(ch));

            while (!isspace(ch) && !inputStream.eof())
            {
                if (ch == '"')
                {
                    do { ss << ch; inputStream.read(&ch, sizeof(ch)); }
                    while (ch != '"');
                    ss << ch;
                    break;
                }

                if (ch != '#')
                {
                    ss << ch;
                    inputStream.read(&ch, sizeof(ch));
                    continue;
                }

                if (!ss.view().empty())
                    break;

                do inputStream.read(&ch, sizeof(ch)); 
                while (ch != '#');

                inputStream.read(&ch, sizeof(ch));
            }

            if (!ss.view().empty())
                break;
        }

        if (ss.view() == ";")
            return "__JASM__ENDL__";

        std::string s { std::move(ss.str()) };
        if (macros.contains(s))
            return macros.at(s);
        return std::move(s);
    }

    void ClearMacros() { macros.clear(); }

    void AddMacro(std::string_view name, std::string_view expansion)
    {
        if (macros.contains(name.data()))
            LOGW(name, " is already present as a macro. Overwriting...");
        macros.emplace(name, expansion);
    }

    void ProcessMacroFile(const std::string_view file)
    {
        LOG("Processing macro file: ", file);
        std::ifstream in { System::OpenInFile(file) };

        std::string name { Tokenize(in) };
        while (name != JASM_EOF && name != JASM_ENDL)
        {
            std::string expansion { Tokenize(in) };
            if (expansion == JASM_EOF || expansion == JASM_ENDL)
                LOGE(System::LogLevel::High, "Unexpected token in macro file ", file);
            macros.emplace(std::move(name), std::move(expansion));
            name = Tokenize(in);
        }
    }
}
