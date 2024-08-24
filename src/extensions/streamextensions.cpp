#include <cctype>
#include <cstdlib>
#include <deque>
#include <ios>
#include <istream>
#include <sstream>
#include <iostream>
#include <string>

#include "extensions/streamextensions.hpp"
#include "system.hpp"

namespace Extensions::Stream
{
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
                return "EOF";

            while (isspace(ch))
                inputStream.read(&ch, sizeof(ch));

            while (!isspace(ch) && !inputStream.eof())
            {
                if (ch != '#')
                {
                    ss << ch;
                    inputStream.read(&ch, sizeof(ch));
                    continue;
                }

                if (!ss.str().empty())
                    break;

                do inputStream.read(&ch, sizeof(ch)); 
                while (ch != '#');

                inputStream.read(&ch, sizeof(ch));

            }

            if (!ss.str().empty())
                break;
        }

        return ss.str();
    }
}
