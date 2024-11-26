#pragma once

#include "core/IServer.h"

MINET_BEGIN

class Logger;
class Task;

/**
 * @brief The default server implementation.
 */
class BasicServer : public IServer
{
public:
    explicit BasicServer(const Ref<ServerConfig>& config);
    ~BasicServer() override;

    static const char* Identifier()
    {
        return "Basic";
    }

    /**
     * @brief Start the server asynchronously.
     * @return The async task.
     */
    Ref<Task> StartAsync() override;

    void Stop() override;

    void SetOnConnection(const OnConnectionCallback& callback) override
    {
        _onConnectionCallback = callback;
    }

    const char* Name() const override
    {
        return "Minet Basic";
    }

private:
    void _Serve();
    void _DecorateContext(const Ref<HttpContext>& context);

    void _OpenSocket();
    void _CloseSocket();

private:
    Ref<ServerConfig> _config;

    OnConnectionCallback _onConnectionCallback;

    int _listenFd;

    bool _isRunning;
};

MINET_END