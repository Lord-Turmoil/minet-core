#pragma once

#include "minet/common/Base.h"

MINET_BEGIN

struct HttpContext;

/**
 * @brief Interface for request handler.
 * Request handler is responsible for handling incoming HTTP requests.
 */
class IRequestHandler
{
public:
    virtual ~IRequestHandler() = default;

    virtual void Handle(const Ref<HttpContext>& context) = 0;
};

MINET_END
