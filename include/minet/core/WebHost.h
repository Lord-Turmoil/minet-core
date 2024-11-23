#pragma once

#include <mioc/mioc.h>
#include "minet/common/Base.h"

MINET_BEGIN

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

private:
    Ref<IServer> _server;
    Ref<IRequestDispatcher> _dispatcher;
    Ref<mioc::ServiceContainer> _container;
};

MINET_END
