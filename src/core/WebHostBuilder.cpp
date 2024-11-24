#include <fstream>
#include <iostream>

#include "minet/common/Assert.h"
#include "minet/components/BasicDispatcher.h"
#include "minet/components/BasicServer.h"
#include "minet/components/LoggerFactory.h"
#include "minet/core/WebHost.h"
#include "minet/core/WebHostBuilder.h"

MINET_BEGIN

static void _PrintBanner();

WebHostBuilder::WebHostBuilder(std::string appSettingsPath)
    : _appSettingsPath(std::move(appSettingsPath)), _container(mioc::ServiceContainer::New(true))
{
    _PrintBanner();
    _LoadSettings();
    _InitializeComponents();
}

WebHostBuilder& WebHostBuilder::Get(const std::string& path, const Ref<IRequestHandler>& handler)
{
    return _RegisterHandler(path, HttpMethod::GET, handler);
}

WebHostBuilder& WebHostBuilder::Post(const std::string& path, const Ref<IRequestHandler>& handler)
{
    return _RegisterHandler(path, HttpMethod::POST, handler);
}

WebHostBuilder& WebHostBuilder::Error(int statusCode, const Ref<IRequestHandler>& handler)
{
    return _RegisterErrorHandler(statusCode, handler);
}

Ref<WebHost> WebHostBuilder::Build()
{
    // Get logger.
    Ref<ILoggerFactory> loggerFactory = _container->Resolve<ILoggerFactory>();
    MINET_ASSERT(loggerFactory);

    // Create request dispatcher.
    Ref<IRequestDispatcher> dispatcher = _container->Resolve<IRequestDispatcher>();
    MINET_ASSERT(dispatcher);

    // Get server.
    Ref<IServer> server = _container->Resolve<IServer>();
    MINET_ASSERT(server);
    server->SetLogger(loggerFactory->GetLogger("Server"));

    // Build Web host.
    Ref<WebHost> host = CreateRef<WebHost>(new WebHost(server, dispatcher, _container));
    host->SetLogger(loggerFactory->GetLogger("WebHost"));

    return host;
}

WebHostBuilder& WebHostBuilder::_RegisterHandler(const std::string& path, HttpMethod method,
                                                 const Ref<IRequestHandler>& handler)
{
    _container->Resolve<IRequestDispatcher>()->RegisterHandler(path, method, handler);
    return *this;
}

WebHostBuilder& WebHostBuilder::_RegisterErrorHandler(int statusCode, const Ref<IRequestHandler>& handler)
{
    _container->Resolve<IRequestDispatcher>()->RegisterErrorHandler(statusCode, handler);
    return *this;
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
            _config = json::parse(file, nullptr, false, true);
        }
        else
        {
            _config = json::object();
            std::cerr << _appSettingsPath << " not found, use default settings." << '\n';
        }
        file.close();
    }

    if (_config.is_discarded())
    {
        std::cerr << "Failed to parse app settings file: " << _appSettingsPath << '\n';
        abort();
    }

    _LoadServerSettings(_config.value("server", json::object()));
    _LoadLoggingSettings(_config.value("logging", json::object()));
}

void WebHostBuilder::_LoadServerSettings(const nlohmann::json& config)
{
    int port = config.value("port", 5000);
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Invalid port number: " << port << ", use default 5000 instead." << '\n';
        port = 5000;
    }
    _container->AddSingleton<ServerConfig>(CreateRef<ServerConfig>(new ServerConfig{ static_cast<uint16_t>(port) }));

    std::string name = config.value("name", "Basic");
    if (name == BasicServer::Identifier())
    {
        _container->AddSingleton<IServer, BasicServer, ServerConfig>();
        _container->AddSingleton<IRequestDispatcher, BasicDispatcher>();
    }
    else
    {
        std::cerr << "Invalid server name: " << name << '\n';
        abort();
    }
}

void WebHostBuilder::_LoadLoggingSettings(const nlohmann::json& config)
{
    Ref<LoggerConfig> loggerConfig = CreateRef<LoggerConfig>();
    bool hasRoot = false;

    for (auto it = config.begin(); it != config.end(); ++it)
    {
        std::string name = it.key();
        const nlohmann::json& spec = it.value();
        LogLevel level = ParseLogLevel(spec.value("level", "Debug"));
        if (level == LogLevel::Invalid)
        {
            std::cerr << "Invalid log level: " << spec.value("level", "Debug") << ", use default Debug instead."
                      << '\n';
            level = LogLevel::Debug;
        }
        std::vector<std::string> sinks = spec.value("sinks", std::vector<std::string>{ "stdout" });

        if (name == "root")
        {
            loggerConfig->DefaultLevel = level;
            loggerConfig->DefaultSinks = sinks;
            hasRoot = true;
        }
        else
        {
            loggerConfig->Specifications[name] = { name, level, sinks };
        }
    }

    // In case there is no root logger, we will set the default settings.
    if (!hasRoot)
    {
        loggerConfig->DefaultLevel = LogLevel::Debug;
        loggerConfig->DefaultSinks = { "stdout" };
    }

    _container->AddSingleton<LoggerConfig>(loggerConfig);
}

void WebHostBuilder::_InitializeComponents()
{
    // Some components are added when loading the settings.

    _container->AddSingleton<ILoggerFactory, LoggerFactory, LoggerConfig>();

    // Since we will register handlers before build, we have to initialize
    // the dispatcher first.
    auto loggerFactory = _container->Resolve<ILoggerFactory>();
    auto dispatcher = _container->Resolve<IRequestDispatcher>();
    dispatcher->SetLogger(loggerFactory->GetLogger("RequestDispatcher"));
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

    static const char* banner = R"(
              _            __                         
   ____ ___  (_)___  ___  / /_    _________  ________ 
  / __ `__ \/ / __ \/ _ \/ __/   / ___/ __ \/ ___/ _ \
 / / / / / / / / / /  __/ /_    / /__/ /_/ / /  /  __/
/_/ /_/ /_/_/_/ /_/\___/\__/    \___/\____/_/   \___/
------------------------------------------------------
  A C++ HTTP server framework mimicking ASP.NET Core
)";
    std::cout << banner << '\n';
}

MINET_END
