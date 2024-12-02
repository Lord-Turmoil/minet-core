#include "components/MayhemServer.h"

#include "minet/common/Assert.h"
#include "minet/components/Logger.h"
#include "minet/core/HttpContext.h"

#include "threading/Task.h"
#include "utils/Epoll.h"
#include "utils/Network.h"

MINET_BEGIN

const int MayhemServer::MAX_FD = 1024;

MayhemServer::MayhemServer(const Ref<ServerConfig>& config)
    : _config(config), _threadPool(config->Threads, config->Capacity),
      _handles(new Ref<AsyncHttpContextBuilder>[MAX_FD]), _listenFd(0), _epollFd(0), _isRunning(false)
{
}

MayhemServer::~MayhemServer()
{
    delete[] _handles;
}

Ref<threading::Task> MayhemServer::StartAsync()
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

    _OpenEpoll();
    if (!_epollFd)
    {
        _logger->Error("Failed to create epoll");
        _CloseSocket();
        return threading::Task::Completed();
    }

    // Note that _listenFd cannot use EPOLL_ET mode.
    if (epoll::Monitor(_epollFd, _listenFd, EPOLLIN) != 0)
    {
        _logger->Error("Failed to monitor server socket");
        _CloseSocket();
        _CloseEpoll();
        return threading::Task::Completed();
    }

    _isRunning = true;
    Ref<threading::Task> task = threading::Task::Create(BIND_FN(_Serve))->StartAsync();
    _logger->Info("Server started");

    return task;
}

void MayhemServer::Stop()
{
    _logger->Info("Shutting down {} server", Name());
    if (!_isRunning)
    {
        _logger->Warn("Server is not running");
    }
    _isRunning = false;
}

void MayhemServer::_Serve()
{
    static constexpr int MAX_EVENTS = 64;

    network::AcceptData data;
    Ref<HttpContext> context;
    epoll_event events[MAX_EVENTS];

    while (_isRunning)
    {
        int count = epoll::Wait(_epollFd, events, MAX_EVENTS);
        if (count == -1)
        {
            _logger->Error("Failed to wait for epoll events");
            _logger->Error("Unable to serve, shutting down server");
            Stop(); // call stop to shut down gracefully
        }

        for (int i = 0; i < count; i++)
        {
            if (events[i].data.fd == _listenFd)
            {
                // There may be multiple incoming connections.
                while (AcceptSocket(_listenFd, &data))
                {
                    if (data.SocketFd >= MAX_FD)
                    {
                        _logger->Error("Unable to handle more connections");
                        network::CloseSocket(data.SocketFd);
                        break;
                    }

                    if (!_MonitorFd(data.SocketFd))
                    {
                        _logger->Error("Failed to monitor new connection");
                        network::CloseSocket(data.SocketFd);
                    }

                    _handles[data.SocketFd] = CreateRef<AsyncHttpContextBuilder>(data);
                }
            }
            else if (events[i].events & EPOLLIN)
            {
                int fd = events[i].data.fd;

                auto& handle = _handles[fd];
                MINET_ASSERT(handle);

                int r = handle->Parse();
                if (r == 1)
                {
                    _UnmonitorFd(fd); // prevent from triggering again

                    const Ref<HttpContext>& context = handle->GetContext();
                    _DecorateContext(context);
                    if (!_threadPool.Submit([this, context] { _onConnectionCallback(context); }))
                    {
                        _logger->Warn("Server overwhelmed, new connection refused");
                        network::CloseSocket(fd);
                        _handles[fd].reset();
                    }
                }
                else if (r < 0)
                {
                    // error occurred
                    _logger->Error("Failed to create HTTP context: {}", r);
                    network::CloseSocket(fd);
                    _handles[fd].reset();
                    continue;
                }
                // else, continue parsing
            }
        }
    }

    _logger->Debug("Closing server socket");
    _CloseSocket();

    _logger->Debug("Closing epoll");
    _CloseEpoll();

    _logger->Info("{} server shut down", Name());
}

void MayhemServer::_HandleConnection(int fd)
{
    Ref<HttpContext> context;
    if (int r = CreateHttpContext(fd, &context); r == 0)
    {
        _DecorateContext(context);
        _onConnectionCallback(context);
    }
    else
    {
        _logger->Error("Failed to create HTTP context, error code: {}", r);
    }
}

void MayhemServer::_DecorateContext(const Ref<HttpContext>& context) const
{
    context->Response.Headers["Server"] = Name();
}

void MayhemServer::_OpenSocket()
{
    _listenFd = network::OpenSocket(_config->Port);
    if (_listenFd < 0)
    {
        _logger->Error("Failed to open socket: {}", _listenFd);
        _listenFd = 0;
    }
}

void MayhemServer::_CloseSocket()
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

void MayhemServer::_OpenEpoll()
{
    _epollFd = epoll::Create();
    if (_epollFd == -1)
    {
        _logger->Error("Failed to create epoll: {}", _epollFd);
        _epollFd = 0;
    }
}

void MayhemServer::_CloseEpoll()
{
    if (_epollFd == 0)
    {
        return;
    }
    if (epoll::Close(_epollFd) != 0)
    {
        _logger->Error("Failed to close epoll");
    }
    _epollFd = 0;
}

bool MayhemServer::_MonitorFd(int fd)
{
    // network::MakeNonBlockingSocket(fd);
    if (epoll::Monitor(_epollFd, fd, EPOLLIN | EPOLLET) != 0)
    {
        _logger->Error("Failed to add fd to epoll");
        return false;
    }
    return true;
}

bool MayhemServer::_UnmonitorFd(int fd)
{
    if (epoll::Unmonitor(_epollFd, fd) != 0)
    {
        _logger->Error("Failed to remove fd from epoll");
        return false;
    }
    return true;
}

MINET_END
