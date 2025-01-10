#include "minet/core/WebHostBuilder.h"

#include "minet/common/Assert.h"
#include "minet/core/WebHost.h"
#include "minet/version.h"

#include "components/LoggerFactory.h"
#include "components/RequestDispatcher.h"

#include "components/BasicServer.h"
#include "components/MayhemServer.h"
#include "components/ThreadedServer.h"

#include "impl/DefaultHandlers.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

MINET_BEGIN

static void _PrintBanner();

WebHostBuilder::WebHostBuilder(Private) : _container(mioc::ServiceContainer::New(true)), _initialized(false)
{
    _PrintBanner();
}

Ref<WebHostBuilder> WebHostBuilder::Create()
{
    return CreateRef<WebHostBuilder>(Private());
}

Ref<WebHostBuilder> WebHostBuilder::UseAppSettings(const std::string& path)
{
    if (_initialized)
    {
        std::cerr << "App settings can only be set once." << '\n';
        exit(1);
    }
    _initialized = true;

    _appSettingsPath = path;
    if (_appSettingsPath == "appsettings.json")
    {
        const char* env = std::getenv("MINET_CORE_ENV");
        if (env)
        {
            _appSettingsPath = std::string("appsettings.") + env + ".json";
        }
    }
    _InitializeComponents();

    return shared_from_this();
}

Ref<WebHostBuilder> WebHostBuilder::Get(const std::string& path, const Ref<IRequestHandler>& handler)
{
    return _RegisterHandler(path, http::HttpMethod::GET, handler);
}

Ref<WebHostBuilder> WebHostBuilder::Post(const std::string& path, const Ref<IRequestHandler>& handler)
{
    return _RegisterHandler(path, http::HttpMethod::POST, handler);
}

Ref<WebHostBuilder> WebHostBuilder::Put(const std::string& path, const Ref<IRequestHandler>& handler)
{
    return _RegisterHandler(path, http::HttpMethod::PUT, handler);
}

Ref<WebHostBuilder> WebHostBuilder::Delete(const std::string& path, const Ref<IRequestHandler>& handler)
{
    return _RegisterHandler(path, http::HttpMethod::DELETE, handler);
}

Ref<WebHostBuilder> WebHostBuilder::Error(int statusCode, const Ref<IRequestHandler>& handler)
{
    return _RegisterErrorHandler(statusCode, handler);
}

Ref<Logger> WebHostBuilder::GetLogger(const std::string& name) const
{
    _Preamble();
    auto loggerFactory = GetServiceContainer()->Resolve<ILoggerFactory>();
    MINET_ASSERT(loggerFactory);
    return loggerFactory->GetLogger(name);
}

Ref<WebHost> WebHostBuilder::Build()
{
    _Preamble();

    // Create request dispatcher.
    Ref<IRequestDispatcher> dispatcher = _container->Resolve<IRequestDispatcher>();
    MINET_ASSERT(dispatcher);

    // Get server.
    Ref<IServer> server = _container->Resolve<IServer>();
    MINET_ASSERT(server);
    server->SetLogger(GetLogger("Server"));

    // Build Web host.
    Ref<WebHost> host = CreateRef<WebHost>(new WebHost(server, dispatcher, _container));
    host->SetLogger(GetLogger("WebHost"));

    return host;
}

Ref<WebHostBuilder> WebHostBuilder::_RegisterHandler(const std::string& path, http::HttpMethod method,
                                                     const Ref<IRequestHandler>& handler)
{
    _Preamble();
    _container->Resolve<IRequestDispatcher>()->RegisterHandler(path, method, handler);
    return shared_from_this();
}

Ref<WebHostBuilder> WebHostBuilder::_RegisterErrorHandler(int statusCode, const Ref<IRequestHandler>& handler)
{
    _Preamble();
    _container->Resolve<IRequestDispatcher>()->RegisterErrorHandler(statusCode, handler);
    return shared_from_this();
}

/**
 * See demo/appsettings.jsonc for more information.
 */
void WebHostBuilder::_LoadSettings()
{
    using namespace nlohmann;

    if (_appSettingsPath.empty())
    {
        _config = json::object();
    }
    else
    {
        std::cout << "Loading app settings from " << _appSettingsPath << '\n';
        std::ifstream file(_appSettingsPath);
        if (file.is_open())
        {
            try
            {
                _config = json::parse(file, nullptr, true, true);
            }
            catch (const std::exception& e)
            {
                std::cerr << "Failed to parse " << _appSettingsPath << '\n';
                std::cerr << "Invalid JSON format: " << e.what() << '\n';
                exit(2);
            }
        }
        else
        {
            _config = json::object();
            std::cerr << _appSettingsPath << " not found, use default settings." << '\n';
        }
        file.close();
    }

    try
    {
        auto serverConfig = LoadServerConfig(_config.value("server", json::object()));
        auto loggerConfig = LoadLoggerConfig(_config.value("logging", json::object()));
        _container->AddSingleton<ServerConfig>(serverConfig);
        _container->AddSingleton<LoggerConfig>(loggerConfig);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to load settings from " << _appSettingsPath << '\n';
        std::cerr << "Invalid app settings format: " << e.what() << '\n';
        exit(2);
    }
}

void WebHostBuilder::_InitializeComponents()
{
    _LoadSettings();

    // Server
    auto serverConfig = _container->Resolve<ServerConfig>();
    if (serverConfig->Name == BasicServer::Identifier())
    {
        _container->AddSingleton<IServer, BasicServer, ServerConfig>();
    }
    else if (serverConfig->Name == ThreadedServer::Identifier())
    {
        _container->AddSingleton<IServer, ThreadedServer, ServerConfig>();
    }
    else if (serverConfig->Name == MayhemServer::Identifier())
    {
        _container->AddSingleton<IServer, MayhemServer, ServerConfig>();
    }
    else
    {
        std::cerr << "Unknown server: " << serverConfig->Name << '\n';
        exit(3);
    }

    // Request dispatcher
    _container->AddSingleton<IRequestDispatcher, RequestDispatcher>();

    // Logger factory
    _container->AddSingleton<ILoggerFactory, LoggerFactory, LoggerConfig>();

    // Initialize loggers.
    auto server = _container->Resolve<IServer>();
    server->SetLogger(GetLogger("Server"));

    auto dispatcher = _container->Resolve<IRequestDispatcher>();
    dispatcher->SetLogger(GetLogger("RequestDispatcher"));

    // Register default handlers.
    impl::RegisterDefaultHandlers(shared_from_this());
}

void WebHostBuilder::_Preamble() const
{
    if (!_initialized)
    {
        std::cerr << "WebHostBuilder not initialized, call UseAppSettings() first." << '\n';
        exit(2);
    }
}

/**
 * Banner generated by Text to ASCII Art Generator (TAAG).
 * @see https://patorjk.com/software/taag/#p=display&f=Slant&t=minet%20core
 */
void _PrintBanner()
{
    static bool printed = false;
    if (printed)
    {
        return;
    }

    static const char* const HEADER = R"(
              _            __                         
   ____ ___  (_)___  ___  / /_    _________  ________ 
  / __ `__ \/ / __ \/ _ \/ __/   / ___/ __ \/ ___/ _ \
 / / / / / / / / / /  __/ /_    / /__/ /_/ / /  /  __/
/_/ /_/ /_/_/_/ /_/\___/\__/    \___/\____/_/   \___/ )";
    static const char* const FOOTER = R"(
------------------------------------------------------
   A C++ HTTP server library mimicking ASP.NET Core   )";
    static const int WIDTH = 54;

    const char* version = MINET_VERSION_STRING;

    std::cout << HEADER << '\n';
    int versionWidth = static_cast<int>(strlen(version) + 1);
    int paddingLeft = (WIDTH /* width or the header */ - versionWidth) / 2;
    int paddingRight = WIDTH - versionWidth - paddingLeft;
    for (int i = 0; i < paddingLeft; ++i)
    {
        std::cout << ' ';
    }
    std::cout << 'v' << version;
    for (int i = 0; i < paddingRight; ++i)
    {
        std::cout << ' ';
    }
    std::cout << FOOTER << '\n';
    printed = true;
}

MINET_END
