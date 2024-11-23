#include "minet/core/IRequestDispatcher.h"
#include "minet/common/Assert.h"
#include "minet/components/Logger.h"
#include "minet/components/LoggerFactory.h"
#include "minet/core/HttpContext.h"
#include "minet/core/IRequestHandler.h"
#include "minet/utils/Parser.h"

MINET_BEGIN

IRequestDispatcher::IRequestDispatcher() : _logger(ILoggerFactory::GetDummyLogger())
{
}

void IRequestDispatcher::Register(const std::string& path, const Ref<IRequestHandler>& handler)
{
    if (path.empty() || (path == "*"))
    {
        if (_defaultHandler)
        {
            _logger->Warn("Default handler is already registered, replacing it with the new one");
        }
        _defaultHandler = handler;
    }
    else
    {
        std::string cleanedPath = utils::http::CleanPath(path);
        if (_handlers.find(cleanedPath) != _handlers.end())
        {
            _logger->Warn("Handler for path '{}' is already registered, replacing it with the new one", cleanedPath);
        }
        // use [] to insert or update
        _handlers[utils::http::CleanPath(path)] = handler;
        _logger->Debug("Registered handler for path '{}'", cleanedPath);
    }
}

void IRequestDispatcher::Dispatch(const Ref<HttpContext>& context)
{
    MINET_ASSERT(context);

    std::string path = utils::http::CleanPath(context->Request.Path);
    _logger->Debug("Dispatching request {}", path);

    auto it = _handlers.find(path);
    if (it != _handlers.end())
    {
        _InvokeHandler(it->second, context);
    }
    else if (_defaultHandler)
    {
        _InvokeHandler(_defaultHandler, context);
    }
    else
    {
        _logger->Warn("No handler found for path '{}'", path);
    }

    DestroyHttpContext(context);
    _logger->Debug("Request {} handled", path);
}

void IRequestDispatcher::SetLogger(const Ref<Logger>& logger)
{
    _logger = logger;
}

MINET_END
