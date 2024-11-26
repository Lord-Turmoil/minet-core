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
    WebHostBuilder();

public:
    /**
     * @warning
     * This must be called before any other methods, and it should only be
     * called once.
     */
    WebHostBuilder& UseAppSettings(const std::string& path = "");

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

    const nlohmann::json& GetAppSettings() const
    {
        return _config;
    }

    Ref<Logger> GetLogger(const std::string& name) const;

    /**
     * Build the web host.
     */
    Ref<WebHost> Build();

private:
    WebHostBuilder& _RegisterHandler(const std::string& path, http::HttpMethod method,
                                     const Ref<IRequestHandler>& handler);
    WebHostBuilder& _RegisterErrorHandler(int statusCode, const Ref<IRequestHandler>& handler);

    void _LoadSettings();
    void _InitializeComponents();

    /**
     * Check if initialized.
     */
    void _Preamble() const;

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

    bool _initialized;
};

MINET_END
