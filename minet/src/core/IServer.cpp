#include "core/IServer.h"

#include "threading/ThreadPool.h"

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

    unsigned threads = config.value("threads", 0);
    if (threads == 0)
    {
        threads = threading::HardwareConcurrency();
    }
    serverConfig->Threads = threads;

    size_t capacity = config.value("capacity", 0u);
    if (capacity == 0)
    {
        capacity = 1024u;
    }
    serverConfig->Capacity = capacity;

    int port = config.value("port", 5000);
    if (port <= 0 || port > 65535)
    {
        throw std::runtime_error("Invalid port number: " + std::to_string(port));
    }
    serverConfig->Port = static_cast<uint16_t>(port);

    return serverConfig;
}

MINET_END
