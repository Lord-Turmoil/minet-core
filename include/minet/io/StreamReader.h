#pragma once

#include "minet/io/Stream.h"

MINET_BEGIN

namespace io
{

class StreamReader
{
public:
    StreamReader(const Ref<Stream>& stream) : _stream(stream)
    {
    }

    virtual ~StreamReader() = default;

    virtual int Read() = 0;
    virtual ssize_t Read(char* buffer, size_t length) = 0;

    virtual bool IsEof() = 0;

protected:
    Ref<Stream> _stream;
};

class BufferedStreamReader final : public StreamReader
{
public:
    BufferedStreamReader(const Ref<Stream>& stream);
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
