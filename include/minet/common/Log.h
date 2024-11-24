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
    Disabled,
    Invalid
};

/**
 * @brief Parse log level from string.
 */
LogLevel ParseLogLevel(std::string level);

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
    std::vector<std::string> Sinks;
};

/**
 * @brief Global logger factory configuration.
 */
struct LoggerConfig
{
    /**
     * Default level for all loggers.
     */
    LogLevel DefaultLevel;

    /**
     * Sinks are the output destinations of the log messages.
     * Use "stdout" for console output and "stderr" for error output.
     * Other names will be treated as file paths.
     */
    std::vector<std::string> DefaultSinks;

    /**
     * Can override specification for each logger.
     */
    std::unordered_map<std::string, LoggerSpecification> Specifications;
};

MINET_END
