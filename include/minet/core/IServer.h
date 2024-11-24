#pragma once

#include <functional>
#include "minet/common/Base.h"
#include "minet/core/ILoggerFactory.h"

MINET_BEGIN

struct HttpContext;
class Task;

/**
 * @brief Configuration for server.
 */
struct ServerConfig
{
    /**
     * @brief The port number to listen on.
     */
    uint16_t Port;
};

/**
 * @brief Interface for server.
 * Server is responsible for accepting incoming connections.
 * When a new connection established, the server will invoke the OnConnection callback.
 */
class IServer
{
    friend class WebHostBuilder;

public:
    using OnConnectionCallback = std::function<void(const Ref<HttpContext>&)>;

    IServer();
    virtual ~IServer() = default;

    virtual Ref<Task> StartAsync() = 0;
    virtual void Stop() = 0;

    virtual void SetOnConnection(const OnConnectionCallback& callback) = 0;

    virtual const char* Name() const = 0;

private:
    // Only used by WebHostBuilder.
    void SetLogger(const Ref<Logger>& logger);

protected:
    Ref<Logger> _logger;
};

MINET_END
