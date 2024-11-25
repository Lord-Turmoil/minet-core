#include "minet/components/BasicServer.h"
#include "minet/common/Assert.h"
#include "minet/components/Logger.h"
#include "minet/core/HttpContext.h"
#include "minet/threading/Task.h"
#include "minet/utils/Network.h"

MINET_BEGIN

BasicServer::BasicServer(const Ref<ServerConfig>& config) : _config(config), _listenFd(0), _isRunning(false)
{
    MINET_ASSERT(config);
}

BasicServer::~BasicServer()
{
    _CloseSocket();
}

Ref<Task> BasicServer::StartAsync()
{
    _logger->Info("Starting {} server on port {}", Name(), _config->Port);

    if (_isRunning)
    {
        _logger->Warn("Server is already running");
        return Task::Completed();
    }

    _OpenSocket();

    if (!_listenFd)
    {
        _logger->Error("Failed to listen on port");
        return Task::Completed();
    }

    if (!_onConnectionCallback)
    {
        _logger->Error("OnConnection callback is not set");
        return Task::Completed();
    }

    _isRunning = true;
    Ref<Task> task = Task::Create(Thread::Create(BIND_FN(_Serve)));
    if (task->StartAsync() != 0)
    {
        _isRunning = false;
        _logger->Error("Failed to start server");
        return Task::Completed();
    }
    _logger->Info("Server started");

    return task;
}

void BasicServer::Stop()
{
    _logger->Info("Shutting down {} server", Name());
    if (!_isRunning)
    {
        _logger->Warn("Server is not running");
    }
    _isRunning = false;
}

void BasicServer::_Serve()
{
    network::AcceptData data;
    Ref<HttpContext> context;
    while (_isRunning)
    {
        while (AcceptSocket(_listenFd, &data) && _isRunning)
        {
            int r = CreateHttpContext(data, &context);
            if (r == 0)
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
    }

    _logger->Debug("Closing server socket");
    _CloseSocket();

    _logger->Info("{} server shut down", Name());
}

void BasicServer::_DecorateContext(const Ref<HttpContext>& context)
{
    context->Response.Headers["Server"] = Name();
}

void BasicServer::_OpenSocket()
{
    _listenFd = network::OpenSocket(_config->Port);
    if (_listenFd < 0)
    {
        _logger->Error("Failed to open socket: {}", _listenFd);
        _listenFd = 0;
    }
}
void BasicServer::_CloseSocket()
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
