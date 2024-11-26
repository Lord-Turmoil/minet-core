#pragma once

#include "minet/io/Stream.h"

MINET_BEGIN

namespace io
{

class StreamReader
{
public:
    explicit StreamReader(const Ref<Stream>& stream) : _stream(stream)
    {
    }

    virtual ~StreamReader() = default;

    StreamReader(const StreamReader& other) = delete;
    StreamReader(StreamReader&& other) noexcept = delete;
    StreamReader& operator=(const StreamReader& other) = delete;
    StreamReader& operator=(StreamReader&& other) noexcept = delete;

    /**
     * @brief Read one character from the stream.
     * @return -1 on EOF, otherwise the char read.
     */
    virtual int Read() = 0;
    virtual ssize_t Read(char* buffer, size_t length) = 0;

    virtual bool IsEof() = 0;

protected:
    Ref<Stream> _stream;
};

class BufferedStreamReader final : public StreamReader
{
public:
    explicit BufferedStreamReader(const Ref<Stream>& stream);
    ~BufferedStreamReader() override;

    int Read() override;
    ssize_t Read(char* buffer, size_t length) override;

    bool IsEof() override
    {
        return _eof;
    }

private:
    size_t _BufferSize() const
    {
        return _tail - _head;
    }
    bool _IsEmpty() const
    {
        return _head == _tail;
    }
    void _Flush();

private:
    static size_t _sBufferSize;
    char* _buffer;
    const char* _head;
    const char* _tail;
    bool _eof;
};

} // namespace io

MINET_END
