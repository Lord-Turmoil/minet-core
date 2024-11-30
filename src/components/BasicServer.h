#pragma once

#include "core/IServer.h"

#include "utils/Network.h"

MINET_BEGIN

/**
 * @brief The default server implementation.
 */
class BasicServer : public IServer
{
public:
    explicit BasicServer(const Ref<ServerConfig>& config);
    ~BasicServer() override;

    BasicServer(const BasicServer&) = delete;
    BasicServer& operator=(const BasicServer&) = delete;
    BasicServer(BasicServer&&) noexcept = default;
    BasicServer& operator=(BasicServer&&) noexcept = default;

    static const char* Identifier()
    {
        return "Basic";
    }

    Ref<threading::Task> StartAsync() override;

    void Stop() override;

    void SetOnConnection(const OnConnectionCallback& callback) override
    {
        _onConnectionCallback = callback;
    }

    const char* Name() const override
    {
        return Identifier();
    }

protected:
    virtual void _OnNewConnection(const network::AcceptData& data);
    void _DecorateContext(const Ref<HttpContext>& context);

private:
    void _Serve();

    void _OpenSocket();
    void _CloseSocket();

protected:
    Ref<ServerConfig> _config;
    OnConnectionCallback _onConnectionCallback;
    int _listenFd;
    bool _isRunning;
};

MINET_END
