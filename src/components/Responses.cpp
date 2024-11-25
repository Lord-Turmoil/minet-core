#include "minet/components/Responses.h"

MINET_BEGIN

HttpResponseWrapper::~HttpResponseWrapper() = default;

TextResponse::TextResponse(HttpResponse* response) : HttpResponseWrapper(response)
{
}

JsonResponse::JsonResponse(HttpResponse* response) : HttpResponseWrapper(response)
{
}

MINET_END
