#include "minet/core/WebHostBuilder.h"
#include "minet/common/Assert.h"
#include "minet/components/BasicDispatcher.h"
#include "minet/components/BasicServer.h"
#include "minet/components/LoggerFactory.h"
#include "minet/core/WebHost.h"

MINET_BEGIN

WebHostBuilder::WebHostBuilder()
{
    // initialize service container (lazy)
    _container = mioc::ServiceContainer::New(true);

    // resolve server config
    _container->AddSingleton<ServerConfig>(CreateRef<ServerConfig>(new ServerConfig{ 5000 }));
    _container->AddSingleton<LoggerConfig>(
        CreateRef<LoggerConfig>(new LoggerConfig{ LogLevel::All, { "stdout", "minet.log" }, {} }));

    // register default services
    _container->AddSingleton<ILoggerFactory, LoggerFactory, LoggerConfig>();
    _container->AddSingleton<IRequestDispatcher, BasicDispatcher>();
    _container->AddSingleton<IServer, BasicServer, ServerConfig>();
}

WebHostBuilder& WebHostBuilder::Register(const std::string& path, const Ref<IRequestHandler>& handler)
{
    _handlers[path] = handler;
    return *this;
}

Ref<WebHost> WebHostBuilder::Build()
{
    // Get logger.
    Ref<ILoggerFactory> loggerFactory = _container->Resolve<ILoggerFactory>();
    MINET_ASSERT(loggerFactory);

    // Create request dispatcher.
    Ref<IRequestDispatcher> dispatcher = _container->Resolve<IRequestDispatcher>();
    MINET_ASSERT(dispatcher);
    dispatcher->SetLogger(loggerFactory->GetLogger("RequestDispatcher"));
    for (const auto& [path, handler] : _handlers)
    {
        dispatcher->Register(path, handler);
    }

    // Get server.
    Ref<IServer> server = _container->Resolve<IServer>();
    MINET_ASSERT(server);
    server->SetLogger(loggerFactory->GetLogger("Server"));

    // Build Web host.
    Ref<WebHost> host = CreateRef<WebHost>(new WebHost(server, dispatcher, _container));
    host->SetLogger(loggerFactory->GetLogger("WebHost"));

    return host;
}

MINET_END
