#pragma once

#include <unordered_map>
#include "minet/common/Base.h"

MINET_BEGIN

struct HttpContext;
class IRequestHandler;
class Logger;

/**
 * @brief
 * Request dispatcher is responsible for dispatching incoming HttpContext
 * to the corresponding handler.
 */
class IRequestDispatcher
{
    friend class WebHostBuilder;

public:
    IRequestDispatcher();
    virtual ~IRequestDispatcher() = default;

    /**
     * @brief Register a handler for the given path.
     * @note
     * If path is empty or a wildcard ("*"), the handler will be used
     * as the default handler.
     * @note
     * If a handler is already registered for the given path, it will
     * be replaced by the new handler.
     */
    void Register(const std::string& path, const Ref<IRequestHandler>& handler);

    /**
     * @brief Dispatch the given HttpContext to the corresponding handler.
     * @note
     * Will fallback to the default handler if no handler is found. And do
     * nothing if no default handler is provided.
     */
    void Dispatch(const Ref<HttpContext>& context);

protected:
    virtual void _InvokeHandler(const Ref<IRequestHandler>& handler, const Ref<HttpContext>& context) = 0;

private:
    // Only used by WebHostBuilder.
    void SetLogger(const Ref<Logger>& logger);

protected:
    Ref<Logger> _logger;

private:
    std::unordered_map<std::string, Ref<IRequestHandler>> _handlers;

    /**
     * @brief Default handler, a.k.a. 404 handler.
     * If provided, this handler will be used when no handler is found
     * for the request.
     */
    Ref<IRequestHandler> _defaultHandler;
};

MINET_END
