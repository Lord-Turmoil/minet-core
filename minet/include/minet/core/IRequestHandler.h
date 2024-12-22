/**
 * @author Tony S.
 * @details Interface for request handler.
 */

#pragma once

#include "minet/common/Base.h"

MINET_BEGIN

struct HttpContext;

/**
 * @brief
 * Request handler is responsible for handling incoming HTTP requests.
 */
class IRequestHandler
{
public:
    virtual ~IRequestHandler() = default;

    virtual int Handle(const Ref<HttpContext>& context) = 0;
};

MINET_END
