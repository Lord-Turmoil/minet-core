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

WebHostBuilder::WebHostBuilder()
    : _appSettingsPath("appsettings.json"), _container(mioc::ServiceContainer::New(true)), _initialized(false)
{
    _PrintBanner();
}

WebHostBuilder& WebHostBuilder::UseAppSettings(const std::string& path)
{
    if (_initialized)
    {
        std::cerr << "App settings can only be set once." << '\n';
        exit(1);
    }
    _initialized = true;

    _appSettingsPath = path;
    _InitializeComponents();

    return *this;
}

WebHostBuilder& WebHostBuilder::Get(const std::string& path, const Ref<IRequestHandler>& handler)
{
    return _RegisterHandler(path, http::HttpMethod::GET, handler);
}

WebHostBuilder& WebHostBuilder::Post(const std::string& path, const Ref<IRequestHandler>& handler)
{
    return _RegisterHandler(path, http::HttpMethod::POST, handler);
}

WebHostBuilder& WebHostBuilder::Error(int statusCode, const Ref<IRequestHandler>& handler)
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

WebHostBuilder& WebHostBuilder::_RegisterHandler(const std::string& path, http::HttpMethod method,
                                                 const Ref<IRequestHandler>& handler)
{
    _Preamble();
    _container->Resolve<IRequestDispatcher>()->RegisterHandler(path, method, handler);
    return *this;
}

WebHostBuilder& WebHostBuilder::_RegisterErrorHandler(int statusCode, const Ref<IRequestHandler>& handler)
{
    _Preamble();
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
    _LoadSettings();

    _container->AddSingleton<ILoggerFactory, LoggerFactory, LoggerConfig>();

    // Since we will register handlers before build, we have to initialize
    // the dispatcher first.
    auto dispatcher = _container->Resolve<IRequestDispatcher>();
    dispatcher->SetLogger(GetLogger("RequestDispatcher"));
}

void WebHostBuilder::_Preamble() const
{
    if (!_initialized)
    {
        std::cerr << "WebHostBuilder not intialized, call UseAppSettings() first." << '\n';
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

    std::cout << HEADER << '\n';
    int versionWidth = static_cast<int>(strlen(Version) + 1);
    int paddingLeft = (WIDTH /* width or the header */ - versionWidth) / 2;
    int paddingRight = WIDTH - versionWidth - paddingLeft;
    for (int i = 0; i < paddingLeft; ++i)
    {
        std::cout << ' ';
    }
    std::cout << 'v' << Version;
    for (int i = 0; i < paddingRight; ++i)
    {
        std::cout << ' ';
    }
    std::cout << FOOTER << '\n';
    printed = true;
}

MINET_END
