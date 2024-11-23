#include "minet/components/LoggerFactory.h"
#include "minet/components/Logger.h"

MINET_BEGIN

LoggerFactory::LoggerFactory(const Ref<LoggerConfig>& config) : _config(config)
{
}

Ref<Logger> LoggerFactory::GetLogger(std::string name)
{
    auto it = _loggers.find(name);
    if (it != _loggers.end())
    {
        return it->second;
    }

    LogLevel level = _config->RootLevel;
    auto it2 = _config->Levels.find(name);
    if (it2 != _config->Levels.end())
    {
        level = it2->second;
    }

    LoggerSpecification spec{ std::move(name), level, _config->Sinks };
    auto logger = CreateRef<Logger>(spec);
    _loggers[spec.Name] = logger;

    return logger;
}


MINET_END
