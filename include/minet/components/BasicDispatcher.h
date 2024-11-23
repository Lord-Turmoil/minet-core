#pragma once

#include "minet/core/IRequestDispatcher.h"

MINET_BEGIN

class BasicDispatcher final : public IRequestDispatcher
{
public:
    ~BasicDispatcher() override = default;

private:
    void _InvokeHandler(const Ref<IRequestHandler>& handler, const Ref<HttpContext>& context) override;

    void _WriteResponse(const Ref<HttpContext>& context);
};

MINET_END
