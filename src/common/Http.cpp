#include "minet/common/Http.h"

MINET_BEGIN

namespace http
{

const char* HttpMethodToString(HttpMethod method)
{
    switch (method)
    {
    case HttpMethod::GET:
        return "GET";
    case HttpMethod::POST:
        return "POST";
    case HttpMethod::PUT:
        return "PUT";
    case HttpMethod::DELETE:
        return "DELETE";
    case HttpMethod::HEAD:
        return "HEAD";
    case HttpMethod::OPTIONS:
        return "OPTIONS";
    case HttpMethod::TRACE:
        return "TRACE";
    case HttpMethod::PATCH:
        return "PATCH";
    }

    return "UNKNOWN";
}

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
    case METHOD_NOT_ALLOWED:
        return "Method Not Allowed";
    case INTERNAL_SERVER_ERROR:
        return "Internal Server Error";
    default:
        return "Unknown";
    }
}

} // namespace http

MINET_END
