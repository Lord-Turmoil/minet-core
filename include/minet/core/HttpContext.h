#pragma once

#include <string>
#include <unordered_map>
#include "minet/common/Base.h"
#include "minet/io/Stream.h"
#include "minet/utils/Network.h"

MINET_BEGIN

/*
 * This part mimics the HTTP context in ASP.NET Core.
 */
enum class HttpMethod
{
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    OPTIONS,
    TRACE,
    PATCH,
    ANY, // a wildcard, used for dispatcher
};

// FIXME: Request and response struct is not well aligned.

struct HttpContext;

struct HttpRequest
{
    HttpMethod Method;

    /**
     * @brief The host of the request, may include port.
     * @note
     * e.g. www.example.com, localhost:8080
     */
    std::string Host;

    /**
     * @brief The request path, should starts with '/' and not end with '/'.
     */
    std::string Path;

    std::unordered_map<std::string, std::string> Headers;
    int ContentLength;
    std::string ContentType;

    std::string Body;

    Ref<io::Stream> BodyStream;
};

struct HttpResponse
{
    int StatusCode;

    std::unordered_map<std::string, std::string> Headers;
    int ContentLength;
    std::string ContentType;

    std::string Body;

    Ref<io::Stream> BodyStream;
};

struct HttpContext
{
    HttpRequest Request;
    HttpResponse Response;

private:
    // This is ugly. But I cannot think of other ways to hold the
    // original socket fd in order to close it.
    friend int CreateHttpContext(const utils::network::AcceptData& data, Ref<HttpContext>* context);
    friend int DestroyHttpContext(const Ref<HttpContext>& context);
    int _socketFd;
};

/**
 * @brief Create HTTP context.
 * @param data The accepted socket data
 * @param context Output new HTTP context.
 * @return 0 on success, otherwise, see @link ParseHttpRequest.
 */
int CreateHttpContext(const utils::network::AcceptData& data, Ref<HttpContext>* context);

/**
 * @brief Destroy a HTTP context after handling.
 * @param context The context to destroy.
 * @return 0 on success, otherwise non-zero.
 */
int DestroyHttpContext(const Ref<HttpContext>& context);

MINET_END
