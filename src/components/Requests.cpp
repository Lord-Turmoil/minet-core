#include "minet/components/Requests.h"

MINET_BEGIN

HttpRequestWrapper::~HttpRequestWrapper() = default;

JsonRequest::JsonRequest(const HttpRequest* request) : HttpRequestWrapper(request)
{
    _json = nlohmann::json::parse(request->Body, nullptr, false, true);
}

MINET_END
