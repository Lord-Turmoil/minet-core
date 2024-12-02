/**
 * @author Tony S.
 * @details A tiny syntax parser for HTTP request.
 */

#pragma once

#include "minet/common/Base.h"

#include <string>

MINET_BEGIN

struct HttpRequest;

namespace http
{

/**
 * @brief Clean up the URL path.
 * If the URL path does not start with '/', add one, and remove trailing '/'.
 * e.g. "abc" -> "/abc", "abc/" -> "/abc", "/abc" -> "/abc", "/abc/" -> "/abc"
 */
std::string CleanPath(const std::string& path);

/**
 * @brief Parse HTTP request.
 * @param request The output request.
 * @return 0 on success, error codes are as follows:
 *   1: Failed to read from the stream.
 *   2: Failed to parse the prologue.
 *   3: Failed to parse one of the headers.
 */
int ParseHttpRequest(HttpRequest* request);

/**
 * @brief Asynchronously parse HTTP request.
 */
class AsyncHttpRequestParser final
{
public:
    AsyncHttpRequestParser(HttpRequest* request);

    /**
     * @brief Feed a character to parse the request.
     * @return
     *      1: The request is done.
     *      0: The request is still in progress.
     *    < 0: Error occurred, value is the error state.
     */
    int Feed(char ch);

private:
    enum class State
    {
        Start = 10, // avoid conflict with io::StreamStatus
        Method,
        Space1,
        Path,
        Space2,
        Version,
        NewLine,
        HeaderKey,
        Colon,
        Space3,
        HeaderValue,
        HeaderNewLine,
        EndLine,
        Body,
        Done,
        Error
    };

    int _ParseStart(char ch);
    int _ParseMethod(char ch);
    int _ParseSpace1(char ch);
    int _ParsePath(char ch);
    int _ParseSpace2(char ch);
    int _ParseVersion(char ch);
    int _ParseNewLine(char ch);
    int _ParseHeaderKey(char ch);
    int _ParseColon(char ch);
    int _ParseSpace3(char ch);
    int _ParseHeaderValue(char ch);
    int _ParseHeaderNewLine(char ch);
    int _ParseEndLine(char ch);
    int _ParseBody(char ch);

    int _ParseDone(char ch);
    int _ParseError(char ch);

    using ParseFunc = int (AsyncHttpRequestParser::*)(char);
    ParseFunc _handler;
    std::string _token;
    std::string _key;

    HttpRequest* _request;
    int _error;
};

} // namespace http

MINET_END
