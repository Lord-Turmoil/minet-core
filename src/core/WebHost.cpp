#include "minet/core/WebHost.h"
#include "minet/common/Assert.h"
#include "minet/core/IRequestDispatcher.h"
#include "minet/core/IServer.h"
#include "minet/threading/Task.h"

MINET_BEGIN

void WebHost::Run()
{
    _server->StartAsync()->Await();
}

WebHost::WebHost(const Ref<IServer>& server, const Ref<IRequestDispatcher>& dispatcher,
                 const Ref<mioc::ServiceContainer>& container)
    : _server(server), _dispatcher(dispatcher), _container(container)
{
    MINET_ASSERT(server);
    MINET_ASSERT(dispatcher);
    MINET_ASSERT(container);

    _server->SetOnConnection([this](const Ref<HttpContext>& context) { _dispatcher->Dispatch(context); });
}

MINET_END
