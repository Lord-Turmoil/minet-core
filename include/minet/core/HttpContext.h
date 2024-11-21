#pragma once

#include <string>
#include <unordered_map>
#include "minet/common/Base.h"

MINET_BEGIN

struct HttpRequest
{
    std::string Url;
    std::unordered_map<std::string, std::string> Headers;
    // body stream
};

struct HttpResponse
{
    int StatusCode;
    std::unordered_map<std::string, std::string> Headers;
    // body stream
};

struct HttpContext
{
    HttpRequest Request;
    HttpResponse Response;
};

MINET_END
