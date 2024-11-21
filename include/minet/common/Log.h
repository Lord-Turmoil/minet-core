#pragma once

#include <unordered_map>
#include <vector>
#include "minet/common/Base.h"

MINET_BEGIN

/**
 * @brief Custom log levels.
 */
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

/**
 * @brief Global logger factory configuration.
 */
struct LoggerConfig
{
    /**
     * Default level for all loggers.
     */
    LogLevel RootLevel;

    /**
     * Sinks are the output destinations of the log messages.
     * Use "stdout" for console output and "stderr" for error output.
     * Other names will be treated as file paths.
     */
    std::vector<std::string> Sinks;

    /**
     * Can set different levels for different loggers.
     */
    std::unordered_map<std::string, LogLevel> Levels;
};

/**
 * @brief Specification for each logger.
 * @note
 * This is a temporary data, so we don't want too much copy.
 */
struct LoggerSpecification
{
    std::string Name;
    LogLevel Level;

    /**
     * This is a reference to the global sinks.
     */
    const std::vector<std::string>& Sinks;
};

MINET_END
