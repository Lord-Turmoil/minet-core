#pragma once

#include <mioc/mioc.h>
#include "minet/common/Base.h"

MINET_BEGIN

class IServer;
class RequestDispatcher;

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
    WebHost(Ref<IServer> server, Ref<RequestDispatcher> dispatcher, Ref<mioc::ServiceContainer> container);

private:
    Ref<IServer> _server;
    Ref<RequestDispatcher> _dispatcher;
    Ref<mioc::ServiceContainer> _container;
};

MINET_END
