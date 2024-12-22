/**
 * @author Tony S.
 * @details Server interface.
 */

#pragma once

#include "minet/core/ILoggerFactory.h"

#include <nlohmann/json.hpp>

#include <functional>

MINET_BEGIN

struct HttpContext;
namespace threading
{
class Task;
}

/**
 * @brief Configuration for server.
 */
struct ServerConfig
{
    /**
     * @brief Which server to use.
     * @note Can be "Basic" or "Mayhem".
     */
    std::string Name;

    /**
     * @brief The port number to listen on.
     */
    uint16_t Port;

    /**
     * @brief The number of threads to use if the server supports multi-threading.
     */
    unsigned Threads;

    /**
     * @brief Request queue size for each thread.
     */
    size_t Capacity;
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

    IServer(const IServer&) = delete;
    IServer& operator=(const IServer&) = delete;
    IServer(IServer&&) noexcept = default;
    IServer& operator=(IServer&&) noexcept = default;

    /**
     * @brief Start the server asynchronously.
     * @return The async task.
     */
    virtual Ref<threading::Task> StartAsync() = 0;

    virtual void Stop() = 0;

    virtual const char* Name() const = 0;

    void SetOnConnection(const OnConnectionCallback& callback)
    {
        _onConnectionCallback = callback;
    }

private:
    // Only used by WebHostBuilder.
    void SetLogger(const Ref<Logger>& logger);

protected:
    Ref<Logger> _logger;
    OnConnectionCallback _onConnectionCallback;
};

/**
 * @brief Load server configuration from JSON object.
 * @param config JSON configuration entry.
 * @return Loaded server config, or nullptr if failed.
 * @throws std::runtime_error if the configuration is invalid.
 */
Ref<ServerConfig> LoadServerConfig(const nlohmann::json& config);

MINET_END
