#include <algorithm>
#include <cctype>
#include <iostream>

#include "minet/common/Log.h"

MINET_BEGIN

const char* Version = "0.1.0";

LogLevel ParseLogLevel(std::string level)
{
    std::transform(level.begin(), level.end(), level.begin(), tolower);
    if (level == "all")
    {
        return LogLevel::All;
    }
    if (level == "fine")
    {
        return LogLevel::Fine;
    }
    if (level == "debug")
    {
        return LogLevel::Debug;
    }
    if (level == "info")
    {
        return LogLevel::Info;
    }
    if (level == "warning")
    {
        return LogLevel::Warning;
    }
    if (level == "error")
    {
        return LogLevel::Error;
    }
    if (level == "critical")
    {
        return LogLevel::Critical;
    }
    if (level == "disabled")
    {
        return LogLevel::Disabled;
    }
    return LogLevel::Invalid;
}

/**
 * @brief Parse the logger specification from the JSON object.
 * @param config JSON entry.
 * @param spec Output logger specification.
 * @param parent To inherit default values.
 * @note This will exit the program if the configuration is invalid.
 */
static void _ParseLoggerSpecification(const nlohmann::json& config, LoggerSpecification* spec,
                                      LoggerSpecification* parent);

Ref<LoggerConfig> LoadLoggerConfig(const nlohmann::json& config)
{
    Ref<LoggerConfig> loggerConfig = CreateRef<LoggerConfig>();

    LoggerSpecification defaultSpec;
    _ParseLoggerSpecification(config, &defaultSpec, nullptr);
    loggerConfig->DefaultLevel = defaultSpec.Level;
    loggerConfig->DefaultPattern = defaultSpec.Pattern;
    loggerConfig->DefaultSinks = defaultSpec.Sinks;

    try
    {
        auto loggers = config.value("loggers", nlohmann::json::object());
        for (auto it = loggers.begin(); it != loggers.end(); ++it)
        {
            LoggerSpecification spec;
            _ParseLoggerSpecification(*it, &spec, &defaultSpec);
            spec.Name = it.key();
            loggerConfig->Specifications.emplace(spec.Name, spec);
        }
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Invalid logger configuration: " + std::string(e.what()));
    }

    return loggerConfig;
}

void _ParseLoggerSpecification(const nlohmann::json& config, LoggerSpecification* spec, LoggerSpecification* parent)
{
    // level
    if (config.find("level") == config.end())
    {
        spec->Level = parent ? parent->Level : LogLevel::Debug;
    }
    else
    {
        auto level = ParseLogLevel(config.value("level", "Debug"));
        if (level == LogLevel::Invalid)
        {
            throw std::runtime_error("Invalid log level: " + config.value("level", "Debug"));
        }
        spec->Level = level;
    }

    // pattern
    spec->Pattern = config.value("pattern", parent ? parent->Pattern : "");

    // sinks
    auto sinks = config.value("sinks", nlohmann::json::object());
    for (auto& entry : sinks)
    {
        LoggerSink sink;
        sink.Pattern = entry.value("pattern", spec->Pattern);
        sink.File = entry.value("file", "");
        if (sink.File.empty())
        {
            continue;
        }
        spec->Sinks.push_back(std::move(sink));
    }
}

MINET_END
