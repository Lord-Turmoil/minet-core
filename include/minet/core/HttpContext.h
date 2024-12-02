/**
 * @author Tony S.
 * @details Context for an HTTP connection.
 */

#pragma once

#include "minet/common/Http.h"

#include "minet/utils/Parser.h"

#include <string>
#include <unordered_map>

MINET_BEGIN

namespace io
{
class Stream;
class StreamReader;
}

namespace network
{
struct AcceptData;
}

struct HttpContext;

using HeaderCollection = std::unordered_map<std::string, std::string>;

struct HttpRequest
{
    http::HttpMethod Method;
    http::HttpVersion Version;

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
 * @brief Create HTTP context from file descriptor alone.
 * @param fd The socket file descriptor.
 * @param context Output new HTTP context.
 * @return 0 on success, otherwise, see @link ParseHttpRequest.
 * @warning In this case, the Host field in request will be "unknown".
 */
int CreateHttpContext(int fd, Ref<HttpContext>* context);

/**
 * @brief Destroy a HTTP context after handling.
 * @param context The context to destroy.
 * @return 0 on success, otherwise non-zero.
 */
int DestroyHttpContext(const Ref<HttpContext>& context);

/**
 * @brief Asynchronously parse HTTP context.
 */
class AsyncHttpContextBuilder
{
public:
    AsyncHttpContextBuilder(const network::AcceptData& data);

    /**
     * @brief Parse the HTTP context.
     * @note This may only be able to parse part of the request.
     * @return
     *      1: Parse completed, call GetContext to get the result.
     *      0: Parse not completed, call again.
     *    < 0: Error occurred.
     */
    int Parse();

    /**
     * @brief Get the parsed HTTP context.
     * @return The parsed HTTP context.
     */
    const Ref<HttpContext>& GetContext()
    {
        return _context;
    }

private:
    Ref<HttpContext> _context;
    Ref<io::StreamReader> _reader;
    http::AsyncHttpRequestParser _parser;
};

MINET_END
