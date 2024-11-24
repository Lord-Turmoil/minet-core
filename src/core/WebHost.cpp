#include "minet/core/WebHost.h"
#include "minet/common/Assert.h"
#include "minet/core/ILoggerFactory.h"
#include "minet/core/IRequestDispatcher.h"
#include "minet/core/IServer.h"
#include "minet/threading/Task.h"
#include "minet/utils/Native.h"

MINET_BEGIN

void WebHost::Run()
{
    _logger->Info("Starting web host");
    _server->StartAsync()->Await();
    _logger->Info("Web host stopped");
    _logger->Info("See you next time!");
}

WebHost::WebHost(const Ref<IServer>& server, const Ref<IRequestDispatcher>& dispatcher,
                 const Ref<mioc::ServiceContainer>& container)
    : _logger(ILoggerFactory::GetDummyLogger()), _server(server), _dispatcher(dispatcher), _container(container)
{
    MINET_ASSERT(server);
    MINET_ASSERT(dispatcher);
    MINET_ASSERT(container);

    _server->SetOnConnection([this](const Ref<HttpContext>& context) { _dispatcher->Dispatch(context); });
    native::SetSignalHandler(SIGINT, BIND_FN(_OnInterrupted));
}

void WebHost::_OnInterrupted()
{
    _logger->Warn("Received abort signal, stopping WebHost");
    _server->Stop();
}

MINET_END
