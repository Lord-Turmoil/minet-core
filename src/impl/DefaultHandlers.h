#pragma once

#include "minet/components/RequestHandler.h"

MINET_BEGIN

class WebHostBuilder;

namespace impl
{

void BadRequestHandler(const TextRequest& request, TextResponse& response);

void MethodNotAllowedHandler(const TextRequest& request, TextResponse& response);

void NotFoundHandler(const TextRequest& request, TextResponse& response);

void InternalServerErrorHandler(const TextRequest& request, TextResponse& response);

void RegisterDefaultHandlers(const Ref<WebHostBuilder>& builder);

} // namespace impl

MINET_END
