#include "minet/core/IRequestDispatcher.h"
#include "minet/common/Assert.h"
#include "minet/components/Logger.h"
#include "minet/components/LoggerFactory.h"
#include "minet/core/IRequestHandler.h"
#include "minet/utils/Http.h"
#include "minet/utils/Parser.h"

MINET_BEGIN

IRequestDispatcher::IRequestDispatcher() : _logger(ILoggerFactory::GetDummyLogger())
{
}

void IRequestDispatcher::RegisterHandler(const std::string& path, http::HttpMethod method,
                                         const Ref<IRequestHandler>& handler)
{
    std::string cleanedPath = http::CleanPath(path);
    auto pathIt = _handlers.find(cleanedPath);
    if (pathIt != _handlers.end())
    {
        auto headerIt = pathIt->second.find(method);
        if (headerIt != pathIt->second.end())
        {
            _logger->Warn("Handler for '{} {}' already registered", HttpMethodToString(method), cleanedPath);
            return;
        }
    }

    _handlers[cleanedPath][method] = handler;
    _logger->Debug("Registered handler for '{} {}'", HttpMethodToString(method), cleanedPath);
}

void IRequestDispatcher::RegisterErrorHandler(int statusCode, const Ref<IRequestHandler>& handler)
{
    if (_errorHandlers.find(statusCode) != _errorHandlers.end())
    {
        _logger->Warn("Handler for status code {} is already registered, replacing it with the new one", statusCode);
    }
    _errorHandlers[statusCode] = handler;
}

void IRequestDispatcher::Dispatch(const Ref<HttpContext>& context)
{
    MINET_ASSERT(context);

    std::string path = http::CleanPath(context->Request.Path);
    _logger->Debug("Dispatching request {}", path);

    // Find handler for the path and method.
    auto pathIt = _handlers.find(path);
    Ref<IRequestHandler> handler;
    int statusCode = http::status::OK;
    if (pathIt != _handlers.end())
    {
        auto methodId = pathIt->second.find(context->Request.Method);
        if (methodId != pathIt->second.end())
        {
            handler = methodId->second;
        }
        else
        {
            statusCode = http::status::METHOD_NOT_ALLOWED;
        }
    }
    else
    {
        statusCode = http::status::NOT_FOUND;
    }

    // Invoke the handler.
    if (handler)
    {
        /*
         * The status code returned here is for the server to handle,
         * e.g. Bad Request when request parsing error occured.
         *
         * 200 does not imply there is no error. There could still be error,
         * but the error is handled by the handler itself.
         */
        statusCode = _InvokeHandler(handler, context);
    }
    else
    {
        _logger->Warn("No handler found for path '{}'", path);
    }

    if (statusCode != http::status::OK)
    {
        _logger->Error("Error handling request {}: {} {}", path, statusCode, http::StatusCodeToDescription(statusCode));
        handler = _GetErrorHandler(statusCode);
        if (handler)
        {
            _InvokeHandler(handler, context);
        }
        else
        {
            _logger->Warn("No error handler for status code {}", statusCode);
        }
    }

    DestroyHttpContext(context);
    _logger->Debug("Request {} handled", path);
}

void IRequestDispatcher::SetLogger(const Ref<Logger>& logger)
{
    _logger = logger;
}

Ref<IRequestHandler> IRequestDispatcher::_GetErrorHandler(int statusCode)
{
    auto it = _errorHandlers.find(statusCode);
    if (it != _errorHandlers.end())
    {
        return it->second;
    }
    return nullptr;
}

MINET_END
