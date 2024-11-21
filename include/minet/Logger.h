#pragma once

#include <spdlog/spdlog.h>

#include "minet/Base.h"

MINET_BEGIN

enum class LogLevel : unsigned char
{
    All,
    Fine,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Disabled
};

struct LoggerConfig
{
    std::string name;
    LogLevel level;

    /**
     * Sinks are the output destinations of the log messages.
     * Use "stdout" for console output and "stderr" for error output.
     * Other names will be treated as file paths.
     */
    std::vector<std::string> sinks;
};

/**
 * @brief A simple logger class that wraps around spdlog.
 */
class Logger
{
public:
    Logger(const std::string& name, LogLevel level, const std::string& sink);
    Logger(const LoggerConfig& config);

    static Ref<Logger> GetLogger(const std::string& name, LogLevel level, const std::string& sink = "stdout");
    static Ref<Logger> GetLogger(const LoggerConfig& config);

public:
    // I prefer Uppercase for the first letter of the function name, so I
    // wrap the original functions from spdlog.

    template <typename T> void Trace(const T& msg)
    {
        _impl->trace(msg);
    }

    template <typename... Args> void Trace(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        _impl->trace(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void Debug(const T& msg)
    {
        _impl->debug(msg);
    }

    template <typename... Args> void Debug(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        _impl->debug(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void Info(const T& msg)
    {
        _impl->info(msg);
    }

    template <typename... Args> void Info(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        _impl->info(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void Warn(const T& msg)
    {
        _impl->warn(msg);
    }

    template <typename... Args> void Warn(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        _impl->warn(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void Error(const T& msg)
    {
        _impl->error(msg);
    }

    template <typename... Args> void Error(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        _impl->error(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void Critical(const T& msg)
    {
        _impl->critical(msg);
    }

    template <typename... Args> void Critical(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        _impl->critical(fmt, std::forward<Args>(args)...);
    }

private:
    void _Init(const LoggerConfig& config);

private:
    Ref<spdlog::logger> _impl;
};

MINET_END
