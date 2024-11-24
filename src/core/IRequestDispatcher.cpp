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

void IRequestDispatcher::RegisterHandler(const std::string& path, HttpMethod method,
                                         const Ref<IRequestHandler>& handler)
{
    std::string cleanedPath = utils::http::CleanPath(path);
    if (_handlers.find(cleanedPath) != _handlers.end())
    {
        _logger->Warn("Handler for path '{}' is already registered, replacing it with the new one", cleanedPath);
    }
    // use [] to insert or update
    _handlers[utils::http::CleanPath(path)] = { method, handler };
    _logger->Debug("Registered handler for path '{}'", cleanedPath);
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

    std::string path = utils::http::CleanPath(context->Request.Path);
    _logger->Debug("Dispatching request {}", path);

    // Find handler for the path and method.
    auto it = _handlers.find(path);
    Ref<IRequestHandler> handler;
    int statusCode = http::status::OK;
    if (it != _handlers.end())
    {
        if ((it->second.Method == context->Request.Method) || (it->second.Method == HttpMethod::ANY))
        {
            handler = it->second.Handler;
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
