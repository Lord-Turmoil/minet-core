#pragma once

#include <mioc/mioc.h>
#include <unordered_map>
#include "minet/common/Base.h"

MINET_BEGIN

class IRequestHandler;
class RequestDispatcher;
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
     * @brief Register a handler for the given path.
     * @see RequestDispatcher::Register
     */
    void Register(const std::string& path, const Ref<IRequestHandler>& handler);

    /**
     * @brief Get the service container.
     * Just return the underlying container, so that there will be fewer
     * methods. :P
     */
    const Ref<mioc::ServiceContainer>& GetServiceContainer() const
    {
        return _container;
    }

public:
    /**
     * Build the web host.
     */
    Ref<WebHost> Build();

private:
    /**
     * Handlers for the web host.
     */
    std::unordered_map<std::string, Ref<IRequestHandler>> _handlers;

    /**
     * The IOC container with all plugable services.
     */
    Ref<mioc::ServiceContainer> _container;
};

MINET_END
