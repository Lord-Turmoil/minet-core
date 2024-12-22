/**
 * @author Tony S.
 * @details StreamWriter interface and implementations.
 */

#pragma once

#include "io/Stream.h"

#include <string>

MINET_BEGIN

namespace io
{

class StreamWriter
{
public:
    explicit StreamWriter(const Ref<Stream>& stream) : _stream(stream)
    {
    }

    virtual ~StreamWriter() = default;

    StreamWriter(const StreamWriter& other) = delete;
    StreamWriter(StreamWriter&& other) noexcept = delete;
    StreamWriter& operator=(const StreamWriter& other) = delete;
    StreamWriter& operator=(StreamWriter&& other) noexcept = delete;

    virtual int Write(char ch) = 0;
    virtual ssize_t Write(const char* buffer, size_t length) = 0;

    virtual ssize_t Flush() = 0;

    ssize_t Write(const char* buffer);
    ssize_t Write(const std::string& str);

protected:
    Ref<Stream> _stream;
};

class BufferedStreamWriter final : public StreamWriter
{
public:
    // Have to use this to avoid hiding the base class's Write.
    using StreamWriter::Write;

    BufferedStreamWriter(const Ref<Stream>& stream);
    ~BufferedStreamWriter() override;

    int Write(char ch) override;
    ssize_t Write(const char* buffer, size_t length) override;

    ssize_t Flush() override;

private:
    size_t _BufferSize() const
    {
        return _tail - _buffer;
    }

private:
    static size_t _sBufferSize;
    char* _buffer;
    char* _tail;
};

} // namespace io

MINET_END
