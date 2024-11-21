#include "minet/core/RequestDispatcher.h"
#include "minet/common/Assert.h"
#include "minet/components/Logger.h"
#include "minet/core/HttpContext.h"
#include "minet/core/IRequestHandler.h"
#include "minet/utils/UrlUtils.h"

MINET_BEGIN

RequestDispatcher::RequestDispatcher(Ref<Logger> logger) : _logger(logger)
{
    MINET_ASSERT(_logger);
}

void RequestDispatcher::Register(const std::string& path, const Ref<IRequestHandler>& handler)
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
        std::string cleanedPath = utils::CleanUrl(path);
        if (_handlers.find(cleanedPath) != _handlers.end())
        {
            _logger->Warn("Handler for path '{}' is already registered, replacing it with the new one", cleanedPath);
        }
        // use [] to insert or update
        _handlers[utils::CleanUrl(path)] = handler;
        _logger->Debug("Registered handler for path '{}'", cleanedPath);
    }
}

void RequestDispatcher::Dispatch(const Ref<HttpContext>& context)
{
    MINET_ASSERT(context);

    std::string path = utils::CleanUrl(context->Request.Url);
    auto it = _handlers.find(path);
    if (it != _handlers.end())
    {
        it->second->Handle(context);
    }
    else if (_defaultHandler)
    {
        _defaultHandler->Handle(context);
    }
    else
    {
        _logger->Warn("No handler found for path '{}'", path);
    }
}

MINET_END
