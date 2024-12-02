#include "io/StreamReader.h"

#include <cstring>

MINET_BEGIN

namespace io
{

size_t BufferedStreamReader::_sBufferSize = 1024;

BufferedStreamReader::BufferedStreamReader(const Ref<Stream>& stream)
    : StreamReader(stream), _buffer(new char[_sBufferSize]), _head(_buffer), _tail(_buffer), _eof(false)
{
}

BufferedStreamReader::~BufferedStreamReader()
{
    delete[] _buffer;
}

int BufferedStreamReader::Read()
{
    _Flush();

    if (IsEof())
    {
        return StreamStatus::EndOfFile;
    }

    if (_IsEmpty())
    {
        return StreamStatus::Empty;
    }

    return *_head++;
}

ssize_t BufferedStreamReader::Read(char* buffer, size_t length)
{
    if (IsEof())
    {
        return StreamStatus::EndOfFile;
    }

    size_t remaining = length;
    while (remaining > 0 && !IsEof())
    {
        _Flush();

        size_t size = std::min(length, _BufferSize());
        if (size == 0)
        {
            break;
        }

        std::memcpy(buffer, _head, size);
        _head += size;
        buffer += size;
        remaining -= size;
    }

    return static_cast<ssize_t>(length - remaining);
}

/**
 * @brief Refresh the buffer.
 * @warning Will not refresh if the buffer is not empty.
 */
void BufferedStreamReader::_Flush()
{
    if (_head != _tail)
    {
        return;
    }

    _head = _buffer;
    ssize_t size = _stream->Read(_buffer, _sBufferSize);
    if (size >= 0)
    {
        _tail = _head + size;
        _eof = (size == 0);
    }
    else if (size == StreamStatus::Again)
    {
        // Do nothing, wait for next read.
    }
    else
    {
        // Error!
        _eof = true;
    }
}

} // namespace io

MINET_END
