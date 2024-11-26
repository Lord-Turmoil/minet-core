#include <minet/minet.h>

#include "io/Stream.h"
#include "utils/Parser.h"

#include "doctest.h"

TEST_CASE("TextRequest Wrapper")
{
    const char REQUEST[] = "POST /some/resource HTTP/1.1\r\n"
                           "Host: localhost:8080\r\n"
                           "User-Agent: curl/7.68.0\r\n"
                           "Accept: */*\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 13\r\n"
                           "\r\n"
                           "Hello there!\n";
    auto stream = minet::CreateRef<minet::io::BufferInputStream>(REQUEST, sizeof(REQUEST) - 1);
    minet::HttpRequest request{ .BodyStream = stream };

    CHECK_EQ(minet::http::ParseHttpRequest(&request), 0);

    auto wrapper = minet::RequestFormatter<minet::TextRequest>::PreHandle(request);
    CHECK_EQ(wrapper.IsValid(), true);
    CHECK_EQ(wrapper.Text(), "Hello there!\n");
    // Since header use unordered map, the order of headers may be different
    // CHECK_EQ(wrapper.ToString(), REQUEST);
}

TEST_CASE("JsonRequest Wrapper")
{
    const char REQUEST[] = "POST /some/resource HTTP/1.1\r\n"
                           "Host: localhost:8080\r\n"
                           "User-Agent: curl/7.68.0\r\n"
                           "Accept: */*\r\n"
                           "Content-Type: application/json\r\n"
                           "Content-Length: 16\r\n"
                           "\r\n"
                           "{\"key\": \"value\"}";
    auto stream = minet::CreateRef<minet::io::BufferInputStream>(REQUEST, sizeof(REQUEST) - 1);
    minet::HttpRequest request{ .BodyStream = stream };

    CHECK_EQ(minet::http::ParseHttpRequest(&request), 0);

    auto wrapper = minet::RequestFormatter<minet::JsonRequest>::PreHandle(request);
    CHECK_EQ(wrapper.IsValid(), true);
    CHECK_EQ(wrapper.Json(), nlohmann::json::object({ { "key", "value" } }));
    // Since header use unordered map, the order of headers may be different
    // CHECK_EQ(wrapper.ToString(), REQUEST);
}
