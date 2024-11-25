#include <sstream>

#include "minet/core/HttpContext.h"
#include "minet/utils/Http.h"
#include "minet/utils/Network.h"
#include "minet/utils/Parser.h"

MINET_BEGIN

int CreateHttpContext(const network::AcceptData& data, Ref<HttpContext>* context)
{
    Ref<HttpContext> ctx = CreateRef<HttpContext>();
    Ref<io::Stream> stream = CreateRef<io::SocketStream>(data.SocketFd);

    ctx->_socketFd = data.SocketFd;

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

int DestroyHttpContext(const Ref<HttpContext>& context)
{
    if (context->_socketFd > 0)
    {
        return network::CloseSocket(context->_socketFd);
    }
    return 0;
}

std::string HttpRequest::ToString() const
{
    std::stringstream ss;

    ss << http::HttpMethodToString(Method) << http::entities::SPACE;
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

MINET_END
