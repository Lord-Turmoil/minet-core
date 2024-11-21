#include "minet/components/MayhemServer.h"
#include "minet/common/Assert.h"
#include "minet/components/Logger.h"
#include "minet/threading/Task.h"

MINET_BEGIN

MayhemServer::MayhemServer(const Ref<Logger>& logger, const Ref<ServerConfig>& config)
    : _logger(logger), _isRunning(false)
{
    MINET_ASSERT(logger);
    MINET_ASSERT(config);

    // TODO: prepare config
}

Ref<Task> MayhemServer::StartAsync()
{
    if (!_onConnectionCallback)
    {
        _logger->Error("OnConnection callback is not set");
        return Task::Completed();
    }

    _isRunning = true;
    return Task::Create(Thread::Create(BIND_FN(_Serve)));
}

void MayhemServer::Stop()
{
    if (!_isRunning)
    {
        _logger->Warn("Server is not running");
    }
    _isRunning = false;
}

void MayhemServer::_Serve()
{
    while (_isRunning)
    {
        // TODO: accept connection
    }
}

MINET_END
