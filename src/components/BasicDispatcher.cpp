#include "minet/components/BasicDispatcher.h"
#include "minet/core/HttpContext.h"
#include "minet/core/IRequestHandler.h"
#include "minet/io/StreamWriter.h"
#include "minet/utils/Http.h"

MINET_BEGIN

void BasicDispatcher::_InvokeHandler(const Ref<IRequestHandler>& handler, const Ref<HttpContext>& context)
{
    handler->Handle(context);
    _WriteResponse(context);
}

void BasicDispatcher::_WriteResponse(const Ref<HttpContext>& context)
{
    HttpResponse& response = context->Response;
    io::BufferedStreamWriter writer(response.BodyStream);
    response.ContentLength = static_cast<int>(response.Body.size());

    using namespace http;
    using namespace http::entities;
    using namespace http::status;

    // Start line
    writer.Write(HTTP_VERSION);
    writer.Write(SPACE);
    writer.Write(std::to_string(response.StatusCode));
    writer.Write(SPACE);
    writer.Write(StatusCodeToDescription(response.StatusCode));
    writer.Write(NEW_LINE);

    // Headers
    writer.Write(CONTENT_TYPE);
    writer.Write(COLON);
    writer.Write(response.ContentType);
    writer.Write(NEW_LINE);

    if (response.StatusCode != NO_CONTENT)
    {
        writer.Write(CONTENT_LENGTH);
        writer.Write(COLON);
        writer.Write(std::to_string(response.ContentLength));
        writer.Write(NEW_LINE);
    }

    for (const auto& [key, value] : response.Headers)
    {
        writer.Write(key);
        writer.Write(COLON);
        writer.Write(value);
        writer.Write(NEW_LINE);
    }

    // Body
    if (response.StatusCode != NO_CONTENT)
    {
        writer.Write(NEW_LINE);
        writer.Write(response.Body);
    }
}

MINET_END
