#include "minet/utils/Parser.h"
#include "minet/common/Assert.h"
#include "minet/core/HttpContext.h"
#include "minet/io/StreamReader.h"

MINET_BEGIN

namespace utils::http
{

std::string CleanPath(const std::string& path)
{
    std::string cleanedPath = path;
    if (cleanedPath.front() != '/')
    {
        cleanedPath = "/" + cleanedPath;
    }
    while (cleanedPath.back() == '/')
    {
        cleanedPath.pop_back();
    }
    return cleanedPath;
}

std::string AddressToHost(uint32_t address, uint16_t port)
{
    address = htonl(address);
    std::string host = std::to_string((address >> 24) & 0xFF) + "." + std::to_string((address >> 16) & 0xFF) + "." +
                       std::to_string((address >> 8) & 0xFF) + "." + std::to_string(address & 0xFF);
    if ((port != 0) && (port != 80))
    {
        host += ":" + std::to_string(port);
    }
    return host;
}

/*
 * ===================================================================
 * ------------------------- Request Parser --------------------------
 * ===================================================================
 */

enum class RequestTokenType : uint8_t
{
    Invalid,
    String,
    Colon,
    NewLine,
    End
};

struct RequestToken
{
    RequestTokenType Type;
    std::string Value;
};

class RequestParser
{
public:
    RequestParser(const Ref<io::StreamReader>& reader, HttpRequest* request)
        : _reader(reader), _request(request), _currentToken({ RequestTokenType::Invalid, "" }), _next(0)
    {
    }

    int Parse();

private:
    enum class ParseStatus : uint8_t
    {
        Ok,
        NotMatch,
        Error
    };

    int _Next();
    void _Rewind(char ch);
    void _NextToken();
    void _NextString();

    void _ParsePreamble();
    ParseStatus _ParseStartLine();
    ParseStatus _ParseHeader();
    ParseStatus _ParseBody();

private:
    Ref<io::StreamReader> _reader;
    HttpRequest* _request;

    RequestToken _currentToken;
    char _next;
};

int ParseHttpRequest(HttpRequest* request)
{
    auto reader = CreateRef<io::BufferedStreamReader>(request->BodyStream);
    return RequestParser(reader, request).Parse();
}

int RequestParser::Parse()
{
    if (_ParseStartLine() != ParseStatus::Ok)
    {
        return 2;
    }

    ParseStatus status = _ParseHeader();
    while (status == ParseStatus::Ok)
    {
        status = _ParseHeader();
    }
    if (status == ParseStatus::Error)
    {
        return 3;
    }

    if (_request->ContentLength > 0)
    {
        if (_request->ContentLength > 4096)
        {
            return 4;
        }

        // The new line is already consumed by the header parsing.
        // So we can directly parse the body.
        if (_ParseBody() != ParseStatus::Ok)
        {
            return 5;
        }
    }

    return 0;
}

int RequestParser::_Next()
{
    if (_next == 0)
    {
        return _reader->Read();
    }
    int ch = _next;
    _next = 0;
    return ch;
}

void RequestParser::_Rewind(char ch)
{
    MINET_ASSERT(_next == 0); // cannot rewind twice
    _next = ch;
}

void RequestParser::_NextToken()
{
    static constexpr char delimiters[] = " :\r\n";

    int ch = _Next();
    if (ch == EOF)
    {
        _currentToken.Type = RequestTokenType::End;
        return;
    }

    while (ch == ' ')
    {
        ch = _Next();
    }
    if (ch == EOF)
    {
        _currentToken.Type = RequestTokenType::End;
        return;
    }

    if (ch == ':')
    {
        _currentToken.Type = RequestTokenType::Colon;
        // value is ignored
        return;
    }

    if (ch == '\r')
    {
        ch = _Next();
        if (ch == '\n')
        {
            _currentToken.Type = RequestTokenType::NewLine;
        }
        else
        {
            _currentToken.Type = RequestTokenType::Invalid;
        }
        return;
    }

    _currentToken.Type = RequestTokenType::String;
    _currentToken.Value.clear();
    while ((ch != EOF) && (strchr(delimiters, ch) == nullptr))
    {
        _currentToken.Value.push_back(static_cast<char>(ch));
        ch = _Next();
    }
    if (ch != EOF)
    {
        _Rewind(static_cast<char>(ch));
    }
}

/**
 * @note
 * Since ':' can exists in header value, so we use this parse function
 * to avoid stopping at ':'.
 */
void RequestParser::_NextString()
{
    static constexpr char delimiters[] = " \r\n";

    int ch = _Next();
    if (ch == EOF)
    {
        _currentToken.Type = RequestTokenType::End;
        return;
    }

    while (ch == ' ')
    {
        ch = _Next();
    }
    if (ch == EOF)
    {
        _currentToken.Type = RequestTokenType::End;
        return;
    }

    if (ch == '\r')
    {
        ch = _Next();
        if (ch == '\n')
        {
            _currentToken.Type = RequestTokenType::NewLine;
        }
        else
        {
            _currentToken.Type = RequestTokenType::Invalid;
        }
        return;
    }

    _currentToken.Type = RequestTokenType::String;
    _currentToken.Value.clear();
    while ((ch != EOF) && (strchr(delimiters, ch) == nullptr))
    {
        _currentToken.Value.push_back(static_cast<char>(ch));
        ch = _Next();
    }
    if (ch != EOF)
    {
        _Rewind(static_cast<char>(ch));
    }
}

void RequestParser::_ParsePreamble()
{
    _request->ContentLength = 0;
    _request->ContentType.clear();
}

// StartLine := Method Path Version\r\n
RequestParser::ParseStatus RequestParser::_ParseStartLine()
{
    _NextToken();

    // Method
    if (_currentToken.Type != RequestTokenType::String)
    {
        return ParseStatus::Error;
    }
    if (_currentToken.Value == "GET")
    {
        _request->Method = HttpMethod::GET;
    }
    else if (_currentToken.Value == "POST")
    {
        _request->Method = HttpMethod::POST;
    }
    else
    {
        // TODO: Currently doesn't support more methods.
        return ParseStatus::Error;
    }

    // Path
    _NextToken();
    if (_currentToken.Type != RequestTokenType::String)
    {
        return ParseStatus::Error;
    }
    _request->Path = CleanPath(_currentToken.Value);

    // Version
    _NextToken();
    if (_currentToken.Type != RequestTokenType::String)
    {
        return ParseStatus::Error;
    }
    if (_currentToken.Value != "HTTP/1.1")
    {
        // Currently only support version 1.1
        return ParseStatus::Error;
    }

    _NextToken();
    if (_currentToken.Type != RequestTokenType::NewLine)
    {
        return ParseStatus::Error;
    }

    return ParseStatus::Ok;
}

// Header := Key: Value\r\n
RequestParser::ParseStatus RequestParser::_ParseHeader()
{
    _NextToken();

    // Key: Value
    if (_currentToken.Type != RequestTokenType::String)
    {
        // The last header is followed by a new line, so it's ok
        // to meet new line here. But nothing else.
        if (_currentToken.Type == RequestTokenType::NewLine)
        {
            return ParseStatus::NotMatch;
        }
        return ParseStatus::Error;
    }
    std::string key = _currentToken.Value;

    // :
    _NextToken();
    if (_currentToken.Type != RequestTokenType::Colon)
    {
        return ParseStatus::Error;
    }

    // Value
    _NextString();
    if (_currentToken.Type != RequestTokenType::String)
    {
        return ParseStatus::Error;
    }

    // handle some special headers
    if (key == "Content-Type")
    {
        _request->ContentType = _currentToken.Value;
    }
    else if (key == "Content-Length")
    {
        _request->ContentLength = std::stoi(_currentToken.Value);
    }
    else
    {
        _request->Headers[key] = _currentToken.Value;
    }

    _NextToken();
    if (_currentToken.Type != RequestTokenType::NewLine)
    {
        return ParseStatus::Error;
    }

    return ParseStatus::Ok;
}

RequestParser::ParseStatus RequestParser::_ParseBody()
{
    // FIXME: I'm not sure if I can use std::string::data like this.
    int length = _request->ContentLength;
    _request->Body.resize(length);
    ssize_t size = _reader->Read(_request->Body.data(), length);
    if (size != length)
    {
        return ParseStatus::Error;
    }
    return ParseStatus::Ok;
}

} // namespace utils::http

MINET_END
