/**
 * @author Tony S.
 * @details Stream interface and implementations.
 */

#pragma once

#include <sys/types.h> // ssize_t
#include <vector>
#include "minet/common/Base.h"

MINET_BEGIN

namespace io
{

/**
 * @brief Return value for read/write operations.
 */
namespace StreamStatus
{

enum
{
    Empty = 0,
    EndOfFile = -1,
    Again = -2,
    Error = -3
};

} // namespace StreamStatus

/**
 * @brief Interface for stream.
 * @note Stream doesn't follow RAII, so it require manual close.
 * @note It guarantees that double close is OK.
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

    /**
     * @brief Read from the stream.
     * @param buffer Target buffer.
     * @param length Maximum length to read.
     * @return How many characters read, -1 on failure.
     */
    virtual ssize_t Read(char* buffer, size_t length) = 0;
    virtual ssize_t Write(const char* buffer, size_t length) = 0;

    /**
     * @brief Close the stream.
     * @return 0 on success, otherwise failed.
     */
    virtual int Close() = 0;
};

/**
 * @brief Socket stream.
 * @warning It does not own the file descriptor, so it won't release it.
 */
class SocketStream : public Stream
{
public:
    SocketStream(int fd);
    ~SocketStream() override = default;

    bool IsReadable() const override;
    bool IsWritable() const override;

    ssize_t Read(char* buffer, size_t length) override;
    ssize_t Write(const char* buffer, size_t length) override;

    int Close() override;

protected:
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
        return !_closed;
    }
    bool IsWritable() const override
    {
        return false;
    }

    ssize_t Read(char* buffer, size_t length) override;
    ssize_t Write(const char* buffer, size_t length) override;

    int Close() override;

private:
    std::vector<char> _buffer;
    size_t _offset;
    bool _closed;
};

} // namespace io

MINET_END
