#pragma once

#include <vector>
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
    Stream() = default;
    virtual ~Stream() = default;

    // No copy and move.
    Stream(const Stream& other) = delete;
    Stream(Stream&& other) noexcept = delete;
    Stream& operator=(const Stream& other) = delete;
    Stream& operator=(Stream&& other) noexcept = delete;

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

/**
 * @brief Buffer stream.
 */
class BufferInputStream final : public Stream
{
public:
    BufferInputStream(const char* buffer, size_t length);
    ~BufferInputStream() override = default;

    bool IsReadable() const override
    {
        return true;
    }
    bool IsWritable() const override
    {
        return false;
    }

    ssize_t Read(char* buffer, size_t length) override;
    ssize_t Write(const char* buffer, size_t length) override
    {
        return -1;
    }

private:
    std::vector<char> _buffer;
    size_t _offset = 0;
};

} // namespace io

MINET_END
