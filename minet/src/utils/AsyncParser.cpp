#include "minet/utils/Parser.h"

#include "minet/common/Http.h"
#include "minet/core/HttpContext.h"

#include <cctype>
#include <cstring>

MINET_BEGIN

namespace http
{

namespace details
{

inline bool IsPathChar(char ch)
{
    static const char EXTRA_PATH_CHARS[] = "-._~:/?#[]@!$&'()*+,;=";
    return isalnum(ch) || (strchr(EXTRA_PATH_CHARS, ch) != nullptr);
}

inline bool IsHeaderKeyChar(char ch)
{
    return isalnum(ch) || ch == '-';
}

inline bool IsHeaderValueChar(char ch)
{
    return (((33 <= ch) && (ch <= 126)) || (ch == '\t') || (ch == ' '));
}

} // namespace details

#define TRANSIT(handler)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        _handler = &AsyncHttpRequestParser::handler;                                                                   \
    } while (0)

#define CONTINUE() return 0

#define ACCEPT() return 1

#define REJECT() return _error

#define RAISE_ERROR(state)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        TRANSIT(_ParseError);                                                                                          \
        _error = static_cast<int>(state);                                                                              \
        REJECT();                                                                                                      \
    } while (0)

AsyncHttpRequestParser::AsyncHttpRequestParser(HttpRequest* request)
    : _handler(&AsyncHttpRequestParser::_ParseStart), _request(request)
{
    // Some necessary initialization.
    _request->ContentLength = 0;
}

int AsyncHttpRequestParser::Feed(char ch)
{
    return (this->*_handler)(ch);
}

int AsyncHttpRequestParser::_ParseStart(char ch)
{
    if (isalpha(ch))
    {
        _token.push_back(ch);
        TRANSIT(_ParseMethod);
    }
    else
    {
        RAISE_ERROR(State::Start);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseMethod(char ch)
{
    if (isalpha(ch))
    {
        _token.push_back(ch);
    }
    else if (ch == ' ')
    {
        _request->Method = HttpMethodFromString(_token);
        _token.clear();
        if (_request->Method == HttpMethod::INVALID)
        {
            RAISE_ERROR(State::Method);
        }
        TRANSIT(_ParseSpace1);
    }
    else
    {
        RAISE_ERROR(State::Method);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseSpace1(char ch)
{
    if (ch == '/')
    {
        _token.push_back(ch);
        TRANSIT(_ParsePath);
    }
    else
    {
        RAISE_ERROR(State::Space1);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParsePath(char ch)
{
    if (ch == ' ')
    {
        _request->Path = _token;
        _token.clear();
        TRANSIT(_ParseSpace2);
    }
    else if (details::IsPathChar(ch))
    {
        _token.push_back(ch);
    }
    else
    {
        RAISE_ERROR(State::Path);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseSpace2(char ch)
{
    if (ch == 'H')
    {
        _token.push_back(ch);
        TRANSIT(_ParseVersion);
    }
    else
    {
        RAISE_ERROR(State::Space2);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseVersion(char ch)
{
    size_t length = _token.length();
    switch (length)
    {
    case 1:
    case 2:
        if (ch == 'T')
        {
            _token.push_back(ch);
        }
        else
        {
            RAISE_ERROR(State::Version);
        }
        break;
    case 3:
        if (ch == 'P')
        {
            _token.push_back(ch);
        }
        else
        {
            RAISE_ERROR(State::Version);
        }
        break;
    case 4:
        if (ch == '/')
        {
            _token.push_back(ch);
        }
        else
        {
            RAISE_ERROR(State::Version);
        }
        break;
    case 5:
        if (ch == '1')
        {
            _token.push_back(ch);
        }
        else
        {
            RAISE_ERROR(State::Version);
        }
        break;
    case 6:
        if (ch == '.')
        {
            _token.push_back(ch);
        }
        else
        {
            RAISE_ERROR(State::Version);
        }
        break;
    case 7:
        if (ch == '1')
        {
            _token.push_back(ch);
        }
        else
        {
            RAISE_ERROR(State::Version);
        }
        break;
    case 8:
        if (ch == '\r')
        {
            _request->Version = HttpVersion::HTTP_1_1;
            _token.clear();
            TRANSIT(_ParseNewLine);
        }
        else
        {
            RAISE_ERROR(State::Version);
        }
        break;
    default:
        RAISE_ERROR(State::Version);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseNewLine(char ch)
{
    if (ch == '\n')
    {
        TRANSIT(_ParseHeaderKey);
    }
    else
    {
        RAISE_ERROR(State::NewLine);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseHeaderKey(char ch)
{
    if (ch == '\r')
    {
        TRANSIT(_ParseEndLine);
    }
    else if (details::IsHeaderKeyChar(ch))
    {
        _token.push_back(ch);
    }
    else if (ch == ':')
    {
        _key = _token;
        _token.clear();
        TRANSIT(_ParseColon);
    }
    else
    {
        RAISE_ERROR(State::HeaderKey);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseColon(char ch)
{
    if (ch == ' ')
    {
        TRANSIT(_ParseSpace3);
    }
    else
    {
        RAISE_ERROR(State::Colon);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseSpace3(char ch)
{
    if (details::IsHeaderValueChar(ch))
    {
        _token.push_back(ch);
        TRANSIT(_ParseHeaderValue);
    }
    else
    {
        RAISE_ERROR(State::Space3);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseHeaderValue(char ch)
{
    if (details::IsHeaderValueChar(ch))
    {
        _token.push_back(ch);
    }
    else if (ch == '\r')
    {
        if (_key == "Content-Length")
        {
            _request->ContentLength = std::stoi(_token);
        }
        else if (_key == "Content-Type")
        {
            _request->ContentType = _token;
        }
        else if (_key == "Host")
        {
            _request->Host = _token;
        }
        else
        {
            _request->Headers[_key] = _token;
        }
        _token.clear();
        TRANSIT(_ParseHeaderNewLine);
    }
    else
    {
        RAISE_ERROR(State::HeaderValue);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseHeaderNewLine(char ch)
{
    if (ch == '\n')
    {
        TRANSIT(_ParseHeaderKey);
    }
    else
    {
        RAISE_ERROR(State::HeaderNewLine);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseEndLine(char ch)
{
    if (ch == '\n')
    {
        if (_request->ContentLength == 0)
        {
            TRANSIT(_ParseDone);
            ACCEPT();
        }
        else
        {
            TRANSIT(_ParseBody);
        }
    }
    else
    {
        RAISE_ERROR(State::EndLine);
    }
    return 0;
}

int AsyncHttpRequestParser::_ParseBody(char ch)
{
    if (_request->Body.length() < static_cast<size_t>(_request->ContentLength))
    {
        _request->Body.push_back(ch);
        // End immediately.
        if (_request->Body.length() == static_cast<size_t>(_request->ContentLength))
        {
            TRANSIT(_ParseDone);
            ACCEPT();
        }
    }
    else
    {
        // Too much body.
        RAISE_ERROR(State::Body);
    }
    CONTINUE();
}

int AsyncHttpRequestParser::_ParseDone(char /* ch */)
{
    ACCEPT();
}

int AsyncHttpRequestParser::_ParseError(char /* ch */)
{
    REJECT();
}

} // namespace http

MINET_END