#include "impl/DefaultHandlers.h"

#include "minet/core/WebHostBuilder.h"

MINET_BEGIN

namespace impl
{

static void DefaultErrorHandler(int statusCode, TextResponse& response)
{
    response.SetStatusCode(statusCode);
    response.Text().assign(http::StatusCodeToDescription(statusCode));
}

void BadRequestHandler(const TextRequest& /* request */, TextResponse& response)
{
    DefaultErrorHandler(http::status::BAD_REQUEST, response);
}

void MethodNotAllowedHandler(const TextRequest& /* request */, TextResponse& response)
{
    DefaultErrorHandler(http::status::METHOD_NOT_ALLOWED, response);
}

void NotFoundHandler(const TextRequest& /* request */, TextResponse& response)
{
    DefaultErrorHandler(http::status::NOT_FOUND, response);
}

void InternalServerErrorHandler(const TextRequest& /* request */, TextResponse& response)
{
    DefaultErrorHandler(http::status::INTERNAL_SERVER_ERROR, response);
}

void RegisterDefaultHandlers(const Ref<WebHostBuilder>& builder)
{
    builder->Error(http::status::BAD_REQUEST, RequestHandler::Bind(BadRequestHandler))
        ->Error(http::status::METHOD_NOT_ALLOWED, RequestHandler::Bind(MethodNotAllowedHandler))
        ->Error(http::status::NOT_FOUND, RequestHandler::Bind(NotFoundHandler))
        ->Error(http::status::INTERNAL_SERVER_ERROR, RequestHandler::Bind(InternalServerErrorHandler));
}

}; // namespace impl

MINET_END