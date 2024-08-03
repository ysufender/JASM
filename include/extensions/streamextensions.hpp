#pragma once

#include <string>
#include <fstream>

namespace Extensions::Stream
{
    std::string Tokenize(std::istream& inputStream, bool backwards = false);
}
