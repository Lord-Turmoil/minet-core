/**
 * @author Tony S.
 * @details Wrapper for raw HttpResponse.
 */

#pragma once

#include "minet/core/HttpContext.h"

#include <nlohmann/json.hpp>

MINET_BEGIN

class HttpResponseWrapper
{
public:
    explicit HttpResponseWrapper(HttpResponse* response) : _response(response)
    {
    }

    virtual ~HttpResponseWrapper() = 0;

    void SetStatusCode(int code)
    {
        _response->StatusCode = code;
    }

    HeaderCollection& Headers()
    {
        return _response->Headers;
    }

    HttpResponse& Response()
    {
        return *_response;
    }

    std::string ToString() const
    {
        return _response->ToString();
    }

protected:
    HttpResponse* _response;
};

/**
 * @brief Wrapper for text response.
 */
class TextResponse final : public HttpResponseWrapper
{
public:
    explicit TextResponse(HttpResponse* response);
    ~TextResponse() override = default;

    std::string& Text()
    {
        return _response->Body;
    }
};

/**
 * @brief Wrapper for JSON response.
 * @warning Do not modify the original body in the response.
 */
class JsonResponse final : public HttpResponseWrapper
{
public:
    explicit JsonResponse(HttpResponse* response);
    ~JsonResponse() override = default;

    nlohmann::json& Json()
    {
        return _json;
    }

private:
    nlohmann::json _json;
};

MINET_END
