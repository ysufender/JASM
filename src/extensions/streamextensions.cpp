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

        while (!isspace(ch))
        {
            inputStream.seekg(-2, std::ios::cur);
            inputStream.read(&ch, sizeof(ch));
        }

        return ForwardTokenize(inputStream);
    }

    std::string Tokenize(std::istream& inputStream, bool backwards)
    {
        return backwards ? BackwardTokenize(inputStream) : ForwardTokenize(inputStream);
    }
}