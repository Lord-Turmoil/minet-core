#pragma once

#include <string>
#include <unordered_map>
#include "minet/common/Base.h"
#include "minet/io/Stream.h"
#include "minet/utils/Http.h"
#include "minet/utils/Network.h"

MINET_BEGIN

// FIXME: Request and response struct is not well aligned.

struct HttpContext;

using HeaderCollection = std::unordered_map<std::string, std::string>;

struct HttpRequest
{
    http::HttpMethod Method;

    /**
     * @brief The content length.
     * @note If is 0, will be omitted in the header.
     */
    int ContentLength;

    /**
     * @brief The request path, should starts with '/' and not end with '/'.
     */
    std::string Path;

    /**
     * @brief The host of the request, may include port.
     * @note It is part of the header, but not included in Headers.
     * @note
     * e.g. www.example.com, localhost:8080
     */
    std::string Host;

    /**
     * @brief The content type of the request.
     * @note It is part of the header, but not included in Headers.
     */
    std::string ContentType;

    /**
     * @brief All headers.
     * @warning For now doesn't support multiple headers with the same key.
     */
    HeaderCollection Headers;

    /**
     * @brief The request body.
     */
    std::string Body;

    // Before parsing, request only have BodyStream valid.
    Ref<io::Stream> BodyStream;

    std::string ToString() const;
};

struct HttpResponse
{
    /**
     * @brief The response status code.
     */
    int StatusCode;

    /**
     * @brief Length of the response body.
     * @note If is 0, will be omitted in the header.
     * @warning This is auto-generated, don't try to modify it.
     */
    int ContentLength;

    /**
     * @brief Response content type.
     * @note It is part of the header, but not included in Headers.
     * @warning This is auto-generated, don't try to modify it.
     */
    std::string ContentType;

    /**
     * @brief Collection of all headers.
     * @warning For now doesn't support multiple headers with the same key.
     */
    HeaderCollection Headers;

    /**
     * @brief Response body.
     */
    std::string Body;

    /**
     * @brief Used to write response back to the client.
     */
    Ref<io::Stream> BodyStream;

    std::string ToString() const;
};

struct HttpContext
{
    HttpRequest Request;
    HttpResponse Response;
};

/**
 * @brief Create HTTP context.
 * @param data The accepted socket data
 * @param context Output new HTTP context.
 * @return 0 on success, otherwise, see @link ParseHttpRequest.
 */
int CreateHttpContext(const network::AcceptData& data, Ref<HttpContext>* context);

/**
 * @brief Destroy a HTTP context after handling.
 * @param context The context to destroy.
 * @return 0 on success, otherwise non-zero.
 */
int DestroyHttpContext(const Ref<HttpContext>& context);

MINET_END
