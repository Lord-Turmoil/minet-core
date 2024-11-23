#pragma once

#include "minet/common/Base.h"

MINET_BEGIN

namespace io
{

/**
 * @brief Interface for stream.
 */
class Stream
{
public:
    virtual ~Stream() = default;

    virtual bool IsReadable() const = 0;
    virtual bool IsWritable() const = 0;

    virtual ssize_t Read(char* buffer, size_t length) = 0;
    virtual ssize_t Write(const char* buffer, size_t length) = 0;
};

/**
 * @brief Socket stream.
 * @warning It does not own the file descriptor, so it won't release it.
 */
class SocketStream final : public Stream
{
public:
    SocketStream(int fd);
    ~SocketStream() override = default;

    bool IsReadable() const override;
    bool IsWritable() const override;

    ssize_t Read(char* buffer, size_t length) override;
    ssize_t Write(const char* buffer, size_t length) override;

private:
    int _fd;
};

} // namespace io

MINET_END
