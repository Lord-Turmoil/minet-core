#pragma once

#include <nlohmann/json.hpp>
#include "minet/common/Base.h"
#include "minet/core/HttpContext.h"

MINET_BEGIN

/**
 * @brief Wrapper for text response.
 */
class TextResponse
{
public:
    TextResponse(HttpResponse* response);

    HttpResponse& GetResponse() const
    {
        return *_response;
    }

    std::string& Text() const
    {
        return _response->Body;
    }

private:
    HttpResponse* _response;
};

/**
 * @brief Wrapper for JSON response.
 * @warning Do not modify the original body in the response.
 */
class JsonResponse
{
public:
    JsonResponse(HttpResponse* response);

    HttpResponse& GetResponse() const
    {
        return *_response;
    }

    nlohmann::json& Json()
    {
        return _json;
    }

private:
    HttpResponse* _response;
    nlohmann::json _json;
};

MINET_END
