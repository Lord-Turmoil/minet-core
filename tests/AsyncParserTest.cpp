#include <minet/minet.h>

#include "minet/utils/Parser.h"

#include "io/StreamReader.h"

#include "doctest.h"

int Parse(const minet::Ref<minet::io::StreamReader>& reader, minet::http::AsyncHttpRequestParser& parser)
{
    int ch = reader->Read();
    while (ch > 0)
    {
        int r = parser.Feed(ch);
        if (r < 0)
        {
            return r;
        }
        if (r == 1)
        {
            return 1;
        }
        ch = reader->Read();
    }

    if (ch == minet::io::StreamStatus::EndOfFile)
    {
        return minet::io::StreamStatus::EndOfFile;
    }
    if (ch == minet::io::StreamStatus::Error)
    {
        return minet::io::StreamStatus::Error;
    }

    return 0;
}

TEST_CASE("GET request async")
{
    const char GET_REQUEST[] = "GET /some/resource HTTP/1.1\r\n"
                               "Host: localhost:8080\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Accept: */*\r\n"
                               "\r\n";
    auto stream = minet::CreateRef<minet::io::BufferInputStream>(GET_REQUEST, sizeof(GET_REQUEST) - 1);
    auto reader = minet::CreateRef<minet::io::BufferedStreamReader>(stream);
    minet::HttpRequest request;
    minet::http::AsyncHttpRequestParser parser(&request);

    int r = 0;
    while (r == 0)
    {
        r = Parse(reader, parser);
        CHECK_GE(r, 0);
        if (r == 1)
        {
            break;
        }
    }

    CHECK_EQ(request.Method, minet::http::HttpMethod::GET);
    CHECK_EQ(request.Host, "localhost:8080");
    CHECK_EQ(request.Path, "/some/resource");
    CHECK_EQ(request.Headers.size(), 2);
    CHECK_EQ(request.Headers["User-Agent"], "curl/7.68.0");
    CHECK_EQ(request.Headers["Accept"], "*/*");
}

TEST_CASE("POST request async")
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
    auto reader = minet::CreateRef<minet::io::BufferedStreamReader>(stream);
    minet::HttpRequest request;
    minet::http::AsyncHttpRequestParser parser(&request);

    int r = 0;
    while (r == 0)
    {
        r = Parse(reader, parser);
        CHECK_GE(r, 0);
        if (r == 1)
        {
            break;
        }
    }

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
