#include "minet/components/Responses.h"

MINET_BEGIN

TextResponse::TextResponse(HttpResponse* response) : _response(response)
{
}

JsonResponse::JsonResponse(HttpResponse* response) : _response(response)
{
}

MINET_END
