#pragma once

#include <functional>
#include "minet/components/Requests.h"
#include "minet/components/Responses.h"
#include "minet/core/HttpContext.h"
#include "minet/core/IRequestHandler.h"

MINET_BEGIN

/**
 * @brief Request type adapter.
 * @tparam TRequest Type of the request.
 */
template <typename TRequest> struct RequestFormatter
{
    static TRequest PreHandle(const HttpRequest& request)
    {
        return TRequest(&request);
    }
};

/**
 * @brief Response type adapter.
 * @tparam TResponse Type of the response.
 */
template <typename TResponse> struct ResponseFormatter
{
};

/**
 * @brief Wrapper for request handler.
 * @tparam TRequest Type of the request.
 * @tparam TResponse Type of the response.
 */
template <typename TRequest = TextRequest, typename TResponse = TextResponse>
class RequestHandler : public IRequestHandler
{
public:
    explicit RequestHandler(const std::function<void(const TRequest&, TResponse&)>& handler) : _handler(handler)
    {
    }

    void Handle(const Ref<HttpContext>& context) override
    {
        TRequest request = RequestFormatter<TRequest>::PreHandle(context->Request);
        TResponse response = ResponseFormatter<TResponse>::PreHandle(context->Response);
        _handler(request, response);
        ResponseFormatter<TResponse>::PostHandle(response);
    }

    static Ref<IRequestHandler> Bind(const std::function<void(const TRequest&, TResponse&)>& handler)
    {
        return CreateRef<RequestHandler>(handler);
    }

private:
    std::function<void(const TRequest&, TResponse&)> _handler;
};

/*
 * ===================================================================
 * ------------------------- Specializations -------------------------
 * ===================================================================
 */

template <> struct ResponseFormatter<TextResponse>
{
    static TextResponse PreHandle(HttpResponse& response)
    {
        return { &response };
    }

    static void PostHandle(TextResponse& response)
    {
        response.GetResponse().ContentType = "text/plain";
    }
};

template <> struct ResponseFormatter<JsonResponse>
{
    static JsonResponse PreHandle(HttpResponse& response)
    {
        return { &response };
    }

    static void PostHandle(JsonResponse& response)
    {
        response.GetResponse().ContentType = "application/json";
        response.GetResponse().Body = response.Json().dump(-1, ' ', true, nlohmann::json::error_handler_t::replace);
    }
};

MINET_END
