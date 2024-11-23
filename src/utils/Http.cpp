#include "minet/utils/Http.h"

MINET_BEGIN

namespace http
{

const char* StatusCodeToDescription(int statusCode)
{
    using namespace status;
    switch (statusCode)
    {
    case OK:
        return "OK";
    case NO_CONTENT:
        return "No Content";
    case BAD_REQUEST:
        return "Bad Request";
    case NOT_FOUND:
        return "Not Found";
    case INTERNAL_SERVER_ERROR:
        return "Internal Server Error";
    default:
        return "Unknown";
    }
}

} // namespace http

MINET_END
