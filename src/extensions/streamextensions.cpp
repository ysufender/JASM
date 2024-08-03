#include <cctype>
#include <istream>
#include <sstream>
#include <iostream>

#include "extensions/streamextensions.hpp"

namespace Extensions::Stream
{
    std::string ForwardTokenize(std::istream& inputStream)
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

    std::string BackwardTokenize(std::istream& inputStream)
    {
        char ch { 'g' };

        if (inputStream.tellg() == 0)
            return "SOF";

        while (!isspace(ch) && inputStream.tellg() > 1)
        {
            inputStream.seekg(-2, std::ios::cur);
            inputStream.read(&ch, 1);
        }

        if (inputStream.tellg() == 1)
            inputStream.seekg(0, std::ios::beg);

        return ForwardTokenize(inputStream);
    }

    std::string Tokenize(std::istream& inputStream, bool backwards)
    {
        return backwards ? BackwardTokenize(inputStream) : ForwardTokenize(inputStream);
    }
}
