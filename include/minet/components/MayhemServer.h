#pragma once

#include "minet/common/Base.h"
#include "minet/core/IServer.h"

MINET_BEGIN

class Logger;

/**
 * @brief The default server implementation.
 */
class MayhemServer : public IServer
{
public:
    MayhemServer(const Ref<Logger>& logger, const Ref<ServerConfig>& config);
    ~MayhemServer() override = default;

    /**
     * @brief Start the server asynchronously.
     * @return The asnyc task.
     */
    Ref<Task> StartAsync() override;

    void Stop() override;

    void SetOnConnection(const OnConnectionCallback& callback) override
    {
        _onConnectionCallback = callback;
    }

private:
    void _Serve();

private:
    OnConnectionCallback _onConnectionCallback;
    Ref<Logger> _logger;
    bool _isRunning;
};

MINET_END
