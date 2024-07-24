#pragma once

#include <string>
#include <fstream>

namespace Extensions::Stream
{
    std::string Tokenize(std::ifstream& inputStream, bool backwards = false);
}