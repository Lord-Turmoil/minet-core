#include "minet/core/HttpContext.h"
#include "minet/utils/Parser.h"

MINET_BEGIN

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

MINET_END
