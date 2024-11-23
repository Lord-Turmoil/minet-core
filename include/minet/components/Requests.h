#pragma once

#include <nlohmann/json.hpp>
#include "minet/common/Base.h"
#include "minet/core/HttpContext.h"

MINET_BEGIN

/**
 * @brief Wrapper for plain text request.
 */
class TextRequest
{
public:
    TextRequest(const HttpRequest* request) : _request(request)
    {
    }

    const HttpRequest& Request() const
    {
        return *_request;
    }

    const std::string& Text() const
    {
        return _request->Body;
    }

private:
    const HttpRequest* _request;
};

/**
 * @brief Wrapper for json request.
 */
class JsonRequest
{
public:
    JsonRequest(const HttpRequest* request);

    const HttpRequest& Request() const
    {
        return *_request;
    }

    const nlohmann::json& Json() const
    {
        return _json;
    }

    /**
     * @brief Check if this JSON request is in valid format or not.
     * @return Whether valid.
     */
    bool IsValid() const
    {
        return !_json.is_discarded();
    }

private:
    const HttpRequest* _request;
    nlohmann::json _json;
};

MINET_END
