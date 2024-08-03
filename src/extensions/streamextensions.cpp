#include <cctype>
#include <ios>
#include <istream>
#include <sstream>
#include <iostream>
#include <string>

#include "extensions/streamextensions.hpp"

namespace Extensions::Stream
{
    std::string Tokenize(std::istream& inputStream)
    {
        char ch;
        std::stringstream ss;

        inputStream.read(&ch, sizeof(ch));

        while (true)
        {
            if (inputStream.eof())
                return "EOF";
            while (isspace(ch))
                inputStream.read(&ch, sizeof(ch));
            while (!isspace(ch) && !inputStream.eof())
            {
                if (ch == '#')
                {
                    if (!ss.str().empty())
                        break;

                    do inputStream.read(&ch, sizeof(ch));
                    while (ch != '#');

                    inputStream.read(&ch, sizeof(ch));
                    continue;
                }

                ss << ch;
                inputStream.read(&ch, sizeof(ch));
            }

            if (!ss.str().empty())
                break;
        }

        return std::move(ss.str());
    }
}
