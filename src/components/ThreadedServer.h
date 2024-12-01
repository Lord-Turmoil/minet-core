#pragma once

#include "core/IServer.h"

#include "threading/ThreadPool.h"
#include "utils/Network.h"

MINET_BEGIN

/**
 * @brief Threaded server uses thread pool for async processing.
 */
class ThreadedServer final : public IServer
{
public:
    explicit ThreadedServer(const Ref<ServerConfig>& config);
    ~ThreadedServer() override = default;

    ThreadedServer(const ThreadedServer&) = delete;
    ThreadedServer& operator=(const ThreadedServer&) = delete;
    ThreadedServer(ThreadedServer&&) noexcept = default;
    ThreadedServer& operator=(ThreadedServer&&) noexcept = default;

    static const char* Identifier()
    {
        return "Threaded";
    }

    Ref<threading::Task> StartAsync() override;

    void Stop() override;

    const char* Name() const override
    {
        return Identifier();
    }

private:
    void _Serve();

    void _HandleConnection(const network::AcceptData& data);

    void _DecorateContext(const Ref<HttpContext>& context) const;

    void _OpenSocket();
    void _CloseSocket();

private:
    Ref<ServerConfig> _config;

    threading::ThreadPool _threadPool;

    int _listenFd;
    bool _isRunning;
};

MINET_END
