/**
 * @author Tony S.
 * @details Concrete request dispatcher.
 */

#pragma once

#include "core/IRequestDispatcher.h"

MINET_BEGIN

class RequestDispatcher final : public IRequestDispatcher
{
public:
    ~RequestDispatcher() override = default;

private:
    int _InvokeHandler(const Ref<IRequestHandler>& handler, const Ref<HttpContext>& context) override;

    void _WriteResponse(const Ref<HttpContext>& context);
};

MINET_END
