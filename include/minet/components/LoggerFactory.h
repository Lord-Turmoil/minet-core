#pragma once

#include <unordered_map>
#include "minet/common/Log.h"
#include "minet/core/ILoggerFactory.h"

MINET_BEGIN

class LoggerFactory : public ILoggerFactory
{
public:
    LoggerFactory(const Ref<LoggerConfig>& config);
    ~LoggerFactory() override = default;

    Ref<Logger> GetLogger(const std::string& name) override;

private:
    Ref<LoggerConfig> _config;
    std::unordered_map<std::string, Ref<Logger>> _loggers;
};

MINET_END
