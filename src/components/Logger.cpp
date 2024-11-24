#include "minet/components/Logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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
    case LogLevel::Invalid:
        return spdlog::level::off;
    }
    return spdlog::level::off;
}

Logger::Logger(const LoggerSpecification& config)
{
    _Init(config);
}

Logger::~Logger()
{
    if (_impl)
    {
        _impl->flush();
    }
}

void Logger::_Init(const LoggerSpecification& config)
{
    std::vector<spdlog::sink_ptr> logSinks;

    for (const auto& sink : config.Sinks)
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

    _impl = CreateRef<spdlog::logger>(config.Name, begin(logSinks), end(logSinks));
    register_logger(_impl);

    auto level = LogLevelToSpdLogLevel(config.Level);
    _impl->set_level(level);
    _impl->flush_on(level);
}

MINET_END
