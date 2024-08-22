#pragma once

#include <string>
#include <iostream>

#include "system.hpp"

#define StreamPos(stream) \
    std::streamoff addr { stream.tellp() }; \
    if (addr == -1) LOGE(System::LogLevel::High, "Couldn't get the position of stream.");

namespace Extensions::Stream
{
    std::string Tokenize(std::istream& inputStream);
}
