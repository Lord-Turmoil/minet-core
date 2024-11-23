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

    return utils::network::ReadSocket(_fd, buffer, length);
}

ssize_t SocketStream::Write(const char* buffer, size_t length)
{
    if (!IsWritable())
    {
        return -1;
    }

    return utils::network::WriteSocket(_fd, buffer, length);
}

} // namespace io

MINET_END
