#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "minet/Logger.h"

MINET_BEGIN

static spdlog::level::level_enum LogLevelToSpdLogLevel(LogLevel level)
{
    switch (level)
    {
    case LogLevel::All:
    case LogLevel::Fine:
        return spdlog::level::trace;
    case LogLevel::Debug:
        return spdlog::level::debug;
    case LogLevel::Info:
        return spdlog::level::info;
    case LogLevel::Warning:
        return spdlog::level::warn;
    case LogLevel::Error:
        return spdlog::level::err;
    case LogLevel::Critical:
        return spdlog::level::critical;
    case LogLevel::Disabled:
        return spdlog::level::off;
    }
    return spdlog::level::trace;
}

Logger::Logger(const std::string& name, LogLevel level, const std::string& sink)
{
    LoggerConfig config{ name, level, { sink } };
    _Init(config);
}

Logger::Logger(const LoggerConfig& config)
{
    _Init(config);
}

Ref<Logger> Logger::GetLogger(const std::string& name, LogLevel level, const std::string& sink)
{
    return CreateRef<Logger>(name, level, sink);
}

Ref<Logger> Logger::GetLogger(const LoggerConfig& config)
{
    return CreateRef<Logger>(config);
}

void Logger::_Init(const LoggerConfig& config)
{
    std::vector<spdlog::sink_ptr> logSinks;

    for (const auto& sink : config.sinks)
    {
        if (sink == "stdout")
        {
            logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        }
        else if (sink == "stderr")
        {
            logSinks.emplace_back(std::make_shared<spdlog::sinks::stderr_color_sink_mt>());
        }
        else
        {
            logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(sink, true));
        }
    }

    _impl = CreateRef<spdlog::logger>(config.name, begin(logSinks), end(logSinks));
    register_logger(_impl);

    _impl->set_level(LogLevelToSpdLogLevel(config.level));
}

MINET_END
