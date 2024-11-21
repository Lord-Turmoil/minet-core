#pragma once

#include <functional>
#include "minet/common/Base.h"

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
    int Port;
};

/**
 * @brief Interface for server.
 * Server is responsible for accepting incoming connections.
 * When a new connection established, the server will invoke the OnConnection callback.
 */
class IServer
{
public:
    using OnConnectionCallback = std::function<void(const Ref<HttpContext>&)>;

    virtual ~IServer() = default;

    virtual Ref<Task> StartAsync();
    virtual void Stop();

    virtual void SetOnConnection(const OnConnectionCallback& callback) = 0;
};

MINET_END
