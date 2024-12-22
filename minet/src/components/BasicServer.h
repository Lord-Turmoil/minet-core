#pragma once

#include "core/IServer.h"

#include "utils/Network.h"

MINET_BEGIN

/**
 * @brief The default server implementation.
 */
class BasicServer final : public IServer
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

    const char* Name() const override
    {
        return Identifier();
    }

private:
    void _Serve();

    void _DecorateContext(const Ref<HttpContext>& context) const;

    void _OpenSocket();
    void _CloseSocket();

protected:
    Ref<ServerConfig> _config;

    int _listenFd;
    bool _isRunning;
};

MINET_END
