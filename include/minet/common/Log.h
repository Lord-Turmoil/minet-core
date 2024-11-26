#pragma once

#include <nlohmann/json.hpp>
#include <string>
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
 * @brief Configuration for each sink.
 */
struct LoggerSink
{
    /**
     * @brief Log message pattern.
     * @see https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#customizing-format-using-set_pattern
     */
    std::string Pattern;

    /**
     * @brief The output file path.
     * @note Use "stdout" for console output and "stderr" for error output.
     */
    std::string File;
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
    std::string Pattern;

    /**
     * This is a reference to the global sinks.
     */
    std::vector<LoggerSink> Sinks;
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
     * Default pattern for spdlog.
     */
    std::string DefaultPattern;

    /**
     * Sinks are the output destinations of the log messages.
     * Use "stdout" for console output and "stderr" for error output.
     * Other names will be treated as file paths.
     */
    std::vector<LoggerSink> DefaultSinks;

    /**
     * Can override specification for each logger.
     */
    std::unordered_map<std::string, LoggerSpecification> Specifications;
};

/**
 * @brief Load logging configuration from JSON object.
 * @param config JSON configuration entry.
 * @return Loaded logger config, or nullptr if failed.
 * @throws std::runtime_error if the configuration is invalid.
 */
Ref<LoggerConfig> LoadLoggerConfig(const nlohmann::json& config);

MINET_END
