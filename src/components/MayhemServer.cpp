#include "components/MayhemServer.h"

#include "minet/common/Assert.h"
#include "minet/components/Logger.h"
#include "minet/core/HttpContext.h"

#include "threading/Task.h"
#include "utils/Network.h"

MINET_BEGIN

MayhemServer::MayhemServer(const Ref<ServerConfig>& config)
    : BasicServer(config), _threadPool(config->Threads, config->Capacity)
{
}

void MayhemServer::_OnNewConnection(const network::AcceptData& data)
{
    if (!_threadPool.Submit([this, data] { _HandleConnection(data); }))
    {
        _logger->Warn("Server overwhelmed, new connection refused");
        network::CloseSocket(data.SocketFd);
    }
}

void MayhemServer::_HandleConnection(const network::AcceptData& data)
{
    Ref<HttpContext> context;
    if (int r = CreateHttpContext(data, &context); r == 0)
    {
        _DecorateContext(context);
        _logger->Debug("New connection from {}", context->Request.Host);
        _onConnectionCallback(context);
    }
    else
    {
        _logger->Error("Failed to create HTTP context, error code: {}", r);
    }
}

MINET_END
