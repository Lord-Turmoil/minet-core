#pragma once

#include <nlohmann/json.hpp>
#include "minet/common/Base.h"
#include "minet/core/HttpContext.h"

MINET_BEGIN

class HttpRequestWrapper
{
public:
    HttpRequestWrapper(const HttpRequest* request) : _request(request)
    {
    }

    virtual ~HttpRequestWrapper() = 0;

    const HttpRequest& Request() const
    {
        return *_request;
    }

    virtual bool IsValid() const
    {
        return true;
    }

    std::string ToString() const
    {
        return _request->ToString();
    }

protected:
    const HttpRequest* _request;
};

/**
 * @brief Wrapper for plain text request.
 */
class TextRequest : public HttpRequestWrapper
{
public:
    TextRequest(const HttpRequest* request) : HttpRequestWrapper(request)
    {
    }

    const std::string& Text() const
    {
        return _request->Body;
    }
};

/**
 * @brief Wrapper for json request.
 */
class JsonRequest : public HttpRequestWrapper
{
public:
    JsonRequest(const HttpRequest* request);

    const nlohmann::json& Json() const
    {
        return _json;
    }

    /**
     * @brief Check if this JSON request is in valid format or not.
     * @return Whether valid.
     */
    bool IsValid() const override
    {
        return !_json.is_discarded();
    }

private:
    nlohmann::json _json;
};

MINET_END
