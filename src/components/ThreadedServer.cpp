#include "components/ThreadedServer.h"

#include "minet/common/Assert.h"
#include "minet/components/Logger.h"
#include "minet/core/HttpContext.h"

#include "threading/Task.h"
#include "utils/Network.h"

MINET_BEGIN

ThreadedServer::ThreadedServer(const Ref<ServerConfig>& config)
    : _config(config), _threadPool(config->Threads, config->Capacity), _listenFd(0), _isRunning(false)
{
}

Ref<threading::Task> ThreadedServer::StartAsync()
{
    _logger->Info("Starting {} server on port {}", Name(), _config->Port);

    if (_isRunning)
    {
        _logger->Warn("Server is already running");
        return threading::Task::Completed();
    }

    if (!_onConnectionCallback)
    {
        _logger->Error("OnConnection callback is not set");
        return threading::Task::Completed();
    }

    _OpenSocket();

    if (!_listenFd)
    {
        _logger->Error("Failed to listen on port");
        return threading::Task::Completed();
    }

    _isRunning = true;
    Ref<threading::Task> task = threading::Task::Create(BIND_FN(_Serve))->StartAsync();
    _logger->Info("Server started");

    return task;
}

void ThreadedServer::Stop()
{
    _logger->Info("Shutting down {} server", Name());
    if (!_isRunning)
    {
        _logger->Warn("Server is not running");
    }
    _isRunning = false;
}

void ThreadedServer::_Serve()
{
    network::AcceptData data;
    while (_isRunning)
    {
        while (AcceptSocket(_listenFd, &data) && _isRunning)
        {
            if (!_threadPool.Submit([this, data] { _HandleConnection(data); }))
            {
                _logger->Warn("Server overwhelmed, new connection refused");
                network::CloseSocket(data.SocketFd);
            }
        }
    }

    _logger->Debug("Closing server socket");
    _CloseSocket();

    _logger->Info("{} server shut down", Name());
}

void ThreadedServer::_HandleConnection(const network::AcceptData& data)
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

void ThreadedServer::_DecorateContext(const Ref<HttpContext>& context) const
{
    context->Response.Headers["Server"] = Name();
}

void ThreadedServer::_OpenSocket()
{
    _listenFd = network::OpenSocket(_config->Port);
    if (_listenFd < 0)
    {
        _logger->Error("Failed to open socket: {}", _listenFd);
        _listenFd = 0;
    }
}

void ThreadedServer::_CloseSocket()
{
    if (_listenFd == 0)
    {
        return;
    }

    if (network::CloseSocket(_listenFd) != 0)
    {
        _logger->Error("Failed to close socket");
    }
    _listenFd = 0;
}

MINET_END
