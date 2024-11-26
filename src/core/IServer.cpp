#include "core/IServer.h"

MINET_BEGIN

IServer::IServer() : _logger(ILoggerFactory::GetDummyLogger())
{
}

void IServer::SetLogger(const Ref<Logger>& logger)
{
    _logger = logger;
}

Ref<ServerConfig> LoadServerConfig(const nlohmann::json& config)
{
    Ref<ServerConfig> serverConfig = CreateRef<ServerConfig>();

    serverConfig->Name = config.value("name", "Basic");

    int port = config.value("port", 5000);
    if (port <= 0 || port > 65535)
    {
        throw std::runtime_error("Invalid port number: " + std::to_string(port));
    }
    serverConfig->Port = static_cast<uint16_t>(port);

    return serverConfig;
}

MINET_END
