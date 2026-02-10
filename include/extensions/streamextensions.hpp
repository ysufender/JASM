#pragma once

#include <fstream>
#include <string>
#include <iostream>

#include "system.hpp"

#define OStreamPos(stream, varName) \
    std::streamoff varName { stream.tellp() }; \
    if (varName == -1) LOGE(System::LogLevel::High, "Couldn't get the position of stream.");

#define IStreamPos(stream, varName) \
    std::streamoff varName { stream.tellg() }; \
    if (varName == -1) LOGE(System::LogLevel::High, "Couldn't get the position of stream.");

namespace Extensions::Stream
{
    std::string Tokenize(std::istream& inputStream);
    void ClearMacros();
    void AddMacro(std::string_view name, std::string_view expansion);
    void ProcessMacroFile(const std::string_view file);

    class LinkCompatibleStream
    {
        public:
            virtual ~LinkCompatibleStream() = default;
            virtual std::istream& InputStream() = 0;
            virtual std::ostream& OutputStream() = 0;
    };

    class FileStream : public LinkCompatibleStream
    {
        private:
            std::fstream _stream;

        public:
            FileStream() = delete;

            explicit FileStream(const std::filesystem::path& path)
                : _stream(System::OpenIOFile(path))
            { }

            ~FileStream() { _stream.close(); }

            std::istream& InputStream() override { return _stream; }
            std::ostream& OutputStream() override { return _stream; }
    };

    class MemoryStream : public LinkCompatibleStream
    {
        private:
            std::stringstream _stream;

        public:
            MemoryStream() = default;

            std::istream& InputStream() override { return _stream; }
            std::ostream& OutputStream() override { return _stream; }
    };
}
