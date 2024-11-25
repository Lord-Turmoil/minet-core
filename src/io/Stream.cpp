#include "minet/io/Stream.h"
#include "minet/utils/Network.h"

MINET_BEGIN

namespace io
{

SocketStream::SocketStream(int fd) : _fd(fd)
{
}

// TODO: Check if the file descriptor is readable.
bool SocketStream::IsReadable() const
{
    return _fd > 0;
}

// TODO: Check if the file descriptor is writable.
bool SocketStream::IsWritable() const
{
    return _fd > 0;
}

ssize_t SocketStream::Read(char* buffer, size_t length)
{
    if (!IsReadable())
    {
        return -1;
    }

    return network::ReadSocket(_fd, buffer, length);
}

ssize_t SocketStream::Write(const char* buffer, size_t length)
{
    if (!IsWritable())
    {
        return -1;
    }

    return network::WriteSocket(_fd, buffer, length);
}

BufferInputStream::BufferInputStream(const char* buffer, size_t length) : _buffer(buffer, buffer + length), _offset(0)
{
}

ssize_t BufferInputStream::Read(char* buffer, size_t length)
{
    if (_offset >= _buffer.size())
    {
        return EOF;
    }

    size_t size = std::min(length, _buffer.size() - _offset);
    std::copy(_buffer.begin() + _offset, _buffer.begin() + _offset + size, buffer);
    _offset += size;
    return size;
}

} // namespace io

MINET_END
