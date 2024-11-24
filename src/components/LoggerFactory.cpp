#include "minet/components/LoggerFactory.h"
#include "minet/components/Logger.h"

MINET_BEGIN

LoggerFactory::LoggerFactory(const Ref<LoggerConfig>& config) : _config(config)
{
}

Ref<Logger> LoggerFactory::GetLogger(const std::string& name)
{
    if (auto it = _loggers.find(name); it != _loggers.end())
    {
        return it->second;
    }

    Ref<Logger> logger;
    auto it = _config->Specifications.find(name);
    if (it == _config->Specifications.end())
    {
        LoggerSpecification spec{ name, _config->DefaultLevel, _config->DefaultSinks };
        logger = CreateRef<Logger>(spec);
    }
    else
    {
        logger = CreateRef<Logger>(it->second);
    }
    _loggers[name] = logger;

    return logger;
}

MINET_END
