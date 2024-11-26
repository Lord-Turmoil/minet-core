/**
 * @author Tony S.
 * @details Helper to build request handler as endpoint.
 */

#pragma once

#include "minet/core/IRequestHandler.h"

#include "minet/components/Requests.h"
#include "minet/components/Responses.h"
#include "minet/core/HttpContext.h"
#include "minet/common/Http.h"

#include <functional>

MINET_BEGIN

/**
 * @brief Request type adapter.
 * @tparam TRequest Type of the request.
 * @note
 * All specializations should have a static method named PreHandle.
 * It is to construct the request wrapper from the original HttpRequest.
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
 * @note
 * All specializations should have two static methods named PreHandle and PostHandle.
 * PreHandle is to construct the response wrapper from the original HttpResponse.
 * PostHandle is to modify the HttpResponse after the request handler has been executed.
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
class RequestHandler final : public IRequestHandler
{
public:
    using RequestHandlerFn = std::function<void(const TRequest&, TResponse&)>;

    explicit RequestHandler(RequestHandlerFn handler) : _handler(std::move(handler))
    {
    }

    int Handle(const Ref<HttpContext>& context) override
    {
        TRequest request = RequestFormatter<TRequest>::PreHandle(context->Request);
        TResponse response = ResponseFormatter<TResponse>::PreHandle(context->Response);
        if (request.IsValid())
        {
            _handler(request, response);
        }
        else
        {
            return http::status::BAD_REQUEST;
        }
        ResponseFormatter<TResponse>::PostHandle(response);
        return http::status::OK;
    }

    static Ref<IRequestHandler> Bind(RequestHandlerFn handler)
    {
        return CreateRef<RequestHandler>(std::move(handler));
    }

private:
    RequestHandlerFn _handler;
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
        return TextResponse(&response);
    }

    static void PostHandle(TextResponse& response)
    {
        response.Response().ContentType = "text/plain";
    }
};

template <> struct ResponseFormatter<JsonResponse>
{
    static JsonResponse PreHandle(HttpResponse& response)
    {
        return JsonResponse(&response);
    }

    static void PostHandle(JsonResponse& response)
    {
        response.Response().ContentType = "application/json";
        response.Response().Body = response.Json().dump(-1, ' ', true, nlohmann::json::error_handler_t::replace);
    }
};

MINET_END
