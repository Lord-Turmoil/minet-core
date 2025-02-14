/**
 * @author Tony S.
 * @details
 * WebHost is the fundamental host for the server.
 * It manages the server, request dispatcher, and the service container.
 */

#pragma once

#include "minet/common/Base.h"

#include <mioc/mioc.h>

MINET_BEGIN

class Logger;
class IServer;
class IRequestDispatcher;

/**
 * @brief The core of the web server.
 */
class WebHost final
{
    friend class WebHostBuilder;

public:
    /**
     * Main loop of the Web server.
     */
    void Run() const;

    WebHost(const WebHost&) = delete;
    WebHost& operator=(const WebHost&) = delete;
    WebHost(WebHost&&) noexcept = default;
    WebHost& operator=(WebHost&&) noexcept = default;

private:
    WebHost(const Ref<IServer>& server, const Ref<IRequestDispatcher>& dispatcher,
            const Ref<mioc::ServiceContainer>& container);

    // Only used by WebHostBuilder.
    void SetLogger(const Ref<Logger>& logger)
    {
        _logger = logger;
    }

    void _OnInterrupted() const;

private:
    Ref<Logger> _logger;
    Ref<IServer> _server;
    Ref<IRequestDispatcher> _dispatcher;
    Ref<mioc::ServiceContainer> _container;
};

MINET_END
