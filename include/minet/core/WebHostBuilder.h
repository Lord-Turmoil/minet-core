#pragma once

#include <mioc/mioc.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include "minet/common/Base.h"

MINET_BEGIN

class IRequestHandler;
class IRequestDispatcher;
class Logger;
class WebHost;

/**
 * @brief Builder for web host.
 * @note
 * There should only be one instance of this class in the whole program.
 * And one instance of this class should only build one web host at a time.
 */
class WebHostBuilder final
{
public:
    WebHostBuilder(std::string appSettingsPath = "appsettings.json");

public:
    /**
     * @brief Register a handler for the given path.
     * @see @link RequestDispatcher::Register.
     */

    WebHostBuilder& Get(const std::string& path, const Ref<IRequestHandler>& handler);
    WebHostBuilder& Post(const std::string& path, const Ref<IRequestHandler>& handler);
    WebHostBuilder& Error(int statusCode, const Ref<IRequestHandler>& handler);

    /**
     * @brief Get the service container.
     * Just return the underlying container, so that there will be fewer
     * methods. :P
     */
    const Ref<mioc::ServiceContainer>& GetServiceContainer() const
    {
        return _container;
    }

    /**
     * Build the web host.
     */
    Ref<WebHost> Build();

private:
    WebHostBuilder& _RegisterHandler(const std::string& path, HttpMethod method, const Ref<IRequestHandler>& handler);
    WebHostBuilder& _RegisterErrorHandler(int statusCode, const Ref<IRequestHandler>& handler);

    void _LoadSettings();
    void _LoadServerSettings(const nlohmann::json& config);
    void _LoadLoggingSettings(const nlohmann::json& config);
    void _InitializeComponents();

private:
    /**
     * Path to the app settings file.
     */
    std::string _appSettingsPath;

    /**
     * Configuration from the app settings file, or default settings.
     */
    nlohmann::json _config;

    /**
     * The IOC container with all pluggable services.
     */
    Ref<mioc::ServiceContainer> _container;
};

MINET_END
