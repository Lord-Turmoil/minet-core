#pragma once

#include <mioc/mioc.h>
#include "minet/common/Base.h"

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
    void Run();

private:
    WebHost(const Ref<IServer>& server, const Ref<IRequestDispatcher>& dispatcher,
            const Ref<mioc::ServiceContainer>& container);

    // Only used by WebHostBuilder.
    void SetLogger(const Ref<Logger>& logger)
    {
        _logger = logger;
    }

    void _OnInterrupted();

private:
    Ref<Logger> _logger;
    Ref<IServer> _server;
    Ref<IRequestDispatcher> _dispatcher;
    Ref<mioc::ServiceContainer> _container;
};

MINET_END
