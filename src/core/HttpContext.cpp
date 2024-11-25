#include <sstream>

#include "minet/core/HttpContext.h"
#include "minet/utils/Http.h"
#include "minet/utils/Parser.h"

MINET_BEGIN

const char* HttpMethodToString(HttpMethod method)
{
    switch (method)
    {
    case HttpMethod::GET:
        return "GET";
    case HttpMethod::POST:
        return "POST";
    case HttpMethod::PUT:
        return "PUT";
    case HttpMethod::DELETE:
        return "DELETE";
    case HttpMethod::HEAD:
        return "HEAD";
    case HttpMethod::OPTIONS:
        return "OPTIONS";
    case HttpMethod::TRACE:
        return "TRACE";
    case HttpMethod::PATCH:
        return "PATCH";
    case HttpMethod::ANY:
        return "*";
    }

    return "UNKNOWN";
}

int CreateHttpContext(const utils::network::AcceptData& data, Ref<HttpContext>* context)
{
    Ref<HttpContext> ctx = CreateRef<HttpContext>();
    Ref<io::Stream> stream = CreateRef<io::SocketStream>(data.SocketFd);

    ctx->_socketFd = data.SocketFd;

    ctx->Request.Host = utils::http::AddressToHost(data.Address.sin_addr.s_addr, data.Address.sin_port);
    ctx->Request.BodyStream = stream;

    ctx->Response.StatusCode = 200;
    ctx->Response.BodyStream = stream;

    int ret = utils::http::ParseHttpRequest(&ctx->Request);
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
        return utils::network::CloseSocket(context->_socketFd);
    }
    return 0;
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
