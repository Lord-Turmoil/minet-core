#pragma once

#include "core/IServer.h"

#include "threading/ThreadPool.h"
#include "utils/Network.h"

MINET_BEGIN

class AsyncHttpContextBuilder;

/**
 * @brief Mayhem server uses epoll and thread pool for async processing.
 * @ref man 7 epoll
 * @note Well, I guess the manual is not that useless after all.
 */
class MayhemServer final : public IServer
{
public:
    explicit MayhemServer(const Ref<ServerConfig>& config);
    ~MayhemServer() override;

    MayhemServer(const MayhemServer&) = delete;
    MayhemServer& operator=(const MayhemServer&) = delete;
    MayhemServer(MayhemServer&&) noexcept = default;
    MayhemServer& operator=(MayhemServer&&) noexcept = default;

    static const char* Identifier()
    {
        return "Mayhem";
    }

    Ref<threading::Task> StartAsync() override;

    void Stop() override;

    const char* Name() const override
    {
        return Identifier();
    }

private:
    void _Serve();

    void _HandleConnection(int fd);

    void _DecorateContext(const Ref<HttpContext>& context) const;

    void _OpenSocket();
    void _CloseSocket();

    void _OpenEpoll();
    void _CloseEpoll();
    bool _MonitorFd(int fd);

private:
    /**
     * @brief The maximum number of file descriptors.
     * @note This is the maximum concurrent requests the server can handle.
     */
    static const int MAX_FD;

    Ref<ServerConfig> _config;

    threading::ThreadPool _threadPool;

    /**
     * @brief One file descriptor for each connection.
     * @note For concurrent request parsing.
     */
    Ref<AsyncHttpContextBuilder>* _handles;

    int _listenFd;
    int _epollFd;
    bool _isRunning;
};

MINET_END
