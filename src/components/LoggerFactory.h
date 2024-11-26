/**
 * @author Tony S.
 * @details Concrete logger factory.
 */

#pragma once

#include "minet/common/Log.h"
#include "minet/core/ILoggerFactory.h"

#include <unordered_map>

MINET_BEGIN

class LoggerFactory final : public ILoggerFactory
{
public:
    explicit LoggerFactory(const Ref<LoggerConfig>& config);
    ~LoggerFactory() override = default;

    Ref<Logger> GetLogger(const std::string& name) override;

private:
    Ref<LoggerConfig> _config;
    std::unordered_map<std::string, Ref<Logger>> _loggers;
};

MINET_END
