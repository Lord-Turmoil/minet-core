#include <minet/minet.h>

#include "io/Stream.h"
#include "utils/Parser.h"

#include "doctest.h"

TEST_CASE("GET request")
{
    const char GET_REQUEST[] = "GET /some/resource HTTP/1.1\r\n"
                               "Host: localhost:8080\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Accept: */*\r\n"
                               "\r\n";
    auto stream = minet::CreateRef<minet::io::BufferInputStream>(GET_REQUEST, sizeof(GET_REQUEST) - 1);
    minet::HttpRequest request{ .BodyStream = stream };

    CHECK_EQ(minet::http::ParseHttpRequest(&request), 0);

    CHECK_EQ(request.Method, minet::http::HttpMethod::GET);
    CHECK_EQ(request.Host, "localhost:8080");
    CHECK_EQ(request.Path, "/some/resource");
    CHECK_EQ(request.Headers.size(), 2);
    CHECK_EQ(request.Headers["User-Agent"], "curl/7.68.0");
    CHECK_EQ(request.Headers["Accept"], "*/*");
}

TEST_CASE("POST request")
{
    const char POST_REQUEST[] = "POST /some/resource HTTP/1.1\r\n"
                                "Host: localhost:8080\r\n"
                                "User-Agent: curl/7.68.0\r\n"
                                "Accept: */*\r\n"
                                "Content-Type: application/json\r\n"
                                "Content-Length: 16\r\n"
                                "\r\n"
                                "{\"key\": \"value\"}";
    auto stream = minet::CreateRef<minet::io::BufferInputStream>(POST_REQUEST, sizeof(POST_REQUEST) - 1);
    minet::HttpRequest request{ .BodyStream = stream };

    CHECK_EQ(minet::http::ParseHttpRequest(&request), 0);

    CHECK_EQ(request.Method, minet::http::HttpMethod::POST);
    CHECK_EQ(request.Host, "localhost:8080");
    CHECK_EQ(request.Path, "/some/resource");
    CHECK_EQ(request.ContentType, "application/json");
    CHECK_EQ(request.ContentLength, 16);
    CHECK_EQ(request.Headers.size(), 2);
    CHECK_EQ(request.Headers["User-Agent"], "curl/7.68.0");
    CHECK_EQ(request.Headers["Accept"], "*/*");
    CHECK_EQ(request.Body, "{\"key\": \"value\"}");
}
