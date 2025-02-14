#include "minet/core/HttpContext.h"

#include "minet/common/Http.h"

#include "io/Stream.h"
#include "io/StreamReader.h"
#include "utils/Network.h"

#include <sstream>

MINET_BEGIN

int CreateHttpContext(const network::AcceptData& data, Ref<HttpContext>* context)
{
    Ref<HttpContext> ctx = CreateRef<HttpContext>();
    Ref<io::Stream> stream = CreateRef<io::SocketStream>(data.SocketFd);

    ctx->Request.Host = network::AddressToHost(data.Address.sin_addr.s_addr, data.Address.sin_port);
    ctx->Request.BodyStream = stream;

    ctx->Response.StatusCode = 200;
    ctx->Response.BodyStream = stream;

    int ret = http::ParseHttpRequest(&ctx->Request);
    if (ret == 0)
    {
        *context = ctx;
    }

    return ret;
}

int CreateHttpContext(int fd, Ref<HttpContext>* context)
{
    Ref<HttpContext> ctx = CreateRef<HttpContext>();
    Ref<io::Stream> stream = CreateRef<io::SocketStream>(fd);

    ctx->Request.Host.assign("unknown");
    ctx->Request.BodyStream = stream;

    ctx->Response.StatusCode = 200;
    ctx->Response.BodyStream = stream;

    int ret = http::ParseHttpRequest(&ctx->Request);
    if (ret == 0)
    {
        *context = ctx;
    }

    return ret;
}

int DestroyHttpContext(const Ref<HttpContext>& context)
{
    // Even if these two streams can be the same, the stream
    // ensures that double close is OK.
    int r1 = context->Request.BodyStream->Close();
    int r2 = context->Response.BodyStream->Close();

    return (r1 == 0) && (r2 == 0);
}

std::string HttpRequest::ToString() const
{
    std::stringstream ss;

    ss << HttpMethodToString(Method) << http::entities::SPACE;
    ss << Path << http::entities::SPACE;
    ss << http::entities::HTTP_VERSION << http::entities::NEW_LINE;

    if (!ContentType.empty())
    {
        ss << "Content-Type" << http::entities::COLON << ContentType << http::entities::NEW_LINE;
    }
    if (ContentLength > 0)
    {
        ss << "Content-Length" << http::entities::COLON << ContentLength << http::entities::NEW_LINE;
    }
    if (!Host.empty())
    {
        ss << "Host" << http::entities::COLON << Host << http::entities::NEW_LINE;
    }
    for (const auto& header : Headers)
    {
        ss << header.first << http::entities::COLON << header.second << http::entities::NEW_LINE;
    }
    // Even if there is no body, we still need to add a new line.
    ss << http::entities::NEW_LINE;

    if (ContentLength > 0)
    {
        ss << Body;
    }

    return ss.str();
}

std::string HttpResponse::ToString() const
{
    std::stringstream ss;

    ss << http::entities::HTTP_VERSION << http::entities::SPACE;
    ss << StatusCode << http::entities::SPACE;
    ss << http::StatusCodeToDescription(StatusCode) << http::entities::NEW_LINE;

    if (!ContentType.empty())
    {
        ss << "Content-Type" << http::entities::COLON << ContentType << http::entities::NEW_LINE;
    }
    if (ContentLength > 0)
    {
        ss << "Content-Length" << http::entities::COLON << ContentLength << http::entities::NEW_LINE;
    }
    for (const auto& header : Headers)
    {
        ss << header.first << http::entities::COLON << header.second << http::entities::NEW_LINE;
    }
    ss << http::entities::NEW_LINE;

    if (ContentLength > 0)
    {
        ss << Body;
    }

    return ss.str();
}

AsyncHttpContextBuilder::AsyncHttpContextBuilder(const network::AcceptData& data)
    : _context(CreateRef<HttpContext>()), _parser(&(_context->Request))
{
    Ref<io::Stream> stream = CreateRef<io::SocketStream>(data.SocketFd);

    _context->Request.Host = network::AddressToHost(data.Address.sin_addr.s_addr, data.Address.sin_port);
    _context->Request.BodyStream = stream;

    _context->Response.StatusCode = 200;
    _context->Response.BodyStream = stream;

    _reader = CreateRef<io::BufferedStreamReader>(stream);
}

int AsyncHttpContextBuilder::Parse()
{
    int ch = _reader->Read();
    while (ch > 0)
    {
        int r = _parser.Feed(ch);
        if (r < 0)
        {
            return r;
        }
        if (r == 1)
        {
            return 1;
        }
        ch = _reader->Read();
    }

    if (ch == io::StreamStatus::EndOfFile)
    {
        return io::StreamStatus::EndOfFile;
    }
    if (ch == io::StreamStatus::Error)
    {
        return io::StreamStatus::Error;
    }

    return 0;
}

MINET_END
