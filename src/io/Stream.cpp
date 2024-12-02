#include "io/Stream.h"

#include "utils/Network.h"

#include <errno.h>

MINET_BEGIN

namespace io
{

/*
 * ===================================================================
 * ------------------------- Socket Stream ---------------------------
 * ===================================================================
 */

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
        return StreamStatus::Error;
    }

    ssize_t read = network::ReadSocket(_fd, buffer, length);
    if (read == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return StreamStatus::Again;
        }
        return StreamStatus::Error;
    }

    return read;
}

ssize_t SocketStream::Write(const char* buffer, size_t length)
{
    if (!IsWritable())
    {
        return StreamStatus::Error;
    }

    ssize_t written = network::WriteSocket(_fd, buffer, length);
    if (written == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return StreamStatus::Again;
        }
        return StreamStatus::Error;
    }

    return written;
}

int SocketStream::Close()
{
    if (_fd > 0)
    {
        int r = network::CloseSocket(_fd);
        _fd = 0;
        return r;
    }
    return 0;
}

/*
 * ===================================================================
 * --------------------- Buffer Input Stream -------------------------
 * ===================================================================
 */

BufferInputStream::BufferInputStream(const char* buffer, size_t length)
    : _buffer(buffer, buffer + length), _offset(0), _closed(false)
{
}

ssize_t BufferInputStream::Read(char* buffer, size_t length)
{
    if (!IsReadable())
    {
        return StreamStatus::Error;
    }

    if (_offset >= _buffer.size())
    {
        return StreamStatus::Error;
    }

    size_t size = std::min(length, _buffer.size() - _offset);
    std::copy(_buffer.begin() + _offset, _buffer.begin() + _offset + size, buffer);
    _offset += size;
    return size;
}

ssize_t BufferInputStream::Write(const char* buffer, size_t length)
{
    return StreamStatus::Error;
}

int BufferInputStream::Close()
{
    _closed = true;
    return 0;
}

} // namespace io

MINET_END
