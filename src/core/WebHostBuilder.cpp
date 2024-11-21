#include "minet/core/WebHostBuilder.h"
#include "minet/common/Assert.h"
#include "minet/components/LoggerFactory.h"
#include "minet/components/MayhemServer.h"
#include "minet/core/RequestDispatcher.h"
#include "minet/core/WebHost.h"

MINET_BEGIN

WebHostBuilder::WebHostBuilder()
{
    // initialize service container (lazy)
    _container = mioc::ServiceContainer::New(true);

    // resolve server config
    _container->AddSingleton<ServerConfig>(CreateRef<ServerConfig>(new ServerConfig{ 5000 }));
    _container->AddSingleton<LoggerConfig>(CreateRef<LoggerConfig>(new LoggerConfig{ LogLevel::Info, { "stdout" } }));

    // register default services
    _container->AddSingleton<ILoggerFactory, LoggerFactory, LoggerConfig>();
    _container->AddTransient<IServer, MayhemServer, Logger, ServerConfig>();
}

void WebHostBuilder::Register(const std::string& path, const Ref<IRequestHandler>& handler)
{
    _handlers[path] = handler;
}

Ref<WebHost> WebHostBuilder::Build()
{
    // Get logger.
    Ref<Logger> logger = _container->Resolve<Logger>();
    MINET_ASSERT(logger);

    // Create request dispatcher.
    Ref<RequestDispatcher> dispatcher = CreateRef<RequestDispatcher>(logger);
    for (const auto& [path, handler] : _handlers)
    {
        dispatcher->Register(path, handler);
    }

    // Get server.
    Ref<IServer> server = _container->Resolve<IServer>();
    MINET_ASSERT(server);

    return CreateRef<WebHost>(new WebHost(server, dispatcher, _container));
}

MINET_END
