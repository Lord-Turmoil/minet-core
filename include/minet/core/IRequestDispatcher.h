#pragma once

#include <unordered_map>
#include "minet/common/Base.h"
#include "minet/core/HttpContext.h"
#include "minet/utils/Http.h"

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
     * If a handler is already registered for the given path, it will
     * be replaced by the new handler.
     */
    void RegisterHandler(const std::string& path, http::HttpMethod method, const Ref<IRequestHandler>& handler);

    void RegisterErrorHandler(int statusCode, const Ref<IRequestHandler>& handler);

    /**
     * @brief Dispatch the given HttpContext to the corresponding handler.
     * @note
     * Will fallback to the default handler if no handler is found. And do
     * nothing if no default handler is provided.
     */
    void Dispatch(const Ref<HttpContext>& context);

protected:
    virtual int _InvokeHandler(const Ref<IRequestHandler>& handler, const Ref<HttpContext>& context) = 0;

private:
    // Only used by WebHostBuilder.
    void SetLogger(const Ref<Logger>& logger);

    Ref<IRequestHandler> _GetErrorHandler(int statusCode);

protected:
    Ref<Logger> _logger;

private:
    using HandlerRegistry = std::unordered_map<std::string, std::unordered_map<http::HttpMethod, Ref<IRequestHandler>>>;
    HandlerRegistry _handlers;
    std::unordered_map<int, Ref<IRequestHandler>> _errorHandlers;
};

MINET_END
