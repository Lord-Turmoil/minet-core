#include "minet/components/Requests.h"

MINET_BEGIN

JsonRequest::JsonRequest(const HttpRequest* request) : _request(request)
{
    _json = nlohmann::json::parse(request->Body, nullptr, false, true);
}

MINET_END
