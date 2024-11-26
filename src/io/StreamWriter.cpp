#include "io/StreamWriter.h"

#include <cstring>

MINET_BEGIN

namespace io
{

ssize_t StreamWriter::Write(const char* buffer)
{
    return Write(buffer, std::strlen(buffer));
}

ssize_t StreamWriter::Write(const std::string& str)
{
    return Write(str.data(), str.size());
}

size_t BufferedStreamWriter::_sBufferSize = 1024;

BufferedStreamWriter::BufferedStreamWriter(const Ref<Stream>& stream)
    : StreamWriter(stream), _buffer(new char[_sBufferSize]), _tail(_buffer)
{
}

BufferedStreamWriter::~BufferedStreamWriter()
{
    Flush(); // ensure buffer is flushed
    delete[] _buffer;
}

int BufferedStreamWriter::Write(char ch)
{
    if (_BufferSize() == _sBufferSize)
    {
        MINET_TRY(Flush());
    }
    *_tail++ = ch;
    return 1;
}

ssize_t BufferedStreamWriter::Write(const char* buffer, size_t length)
{
    size_t remaining = length;
    while (remaining > 0)
    {
        size_t size = std::min(remaining, _sBufferSize - _BufferSize());
        std::memcpy(_tail, buffer, size);
        if (_BufferSize() == _sBufferSize)
        {
            if (Flush() < 0)
            {
                break;
            }
        }
        _tail += size;
        buffer += size;
        remaining -= size;
    }
    return static_cast<ssize_t>(length - remaining);
}

ssize_t BufferedStreamWriter::Flush()
{
    if (_BufferSize() == 0)
    {
        return 0;
    }

    ssize_t r = _stream->Write(_buffer, _BufferSize());
    _tail = _buffer;

    return r;
}

} // namespace io

MINET_END
