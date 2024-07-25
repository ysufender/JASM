#include <sstream>
#include <iostream>

#include "utilities/streamextensions.hpp"

namespace Extensions::Stream
{
    std::string Tokenize(std::ifstream& inputStream, bool backwards)
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