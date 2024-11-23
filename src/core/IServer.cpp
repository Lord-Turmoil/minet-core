#include "minet/core/IServer.h"

MINET_BEGIN

IServer::IServer() : _logger(ILoggerFactory::GetDummyLogger())
{
}

void IServer::SetLogger(const Ref<Logger>& logger)
{
    _logger = logger;
}

MINET_END
