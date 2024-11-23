#include "minet/core/ILoggerFactory.h"
#include "minet/components/Logger.h"

MINET_BEGIN

Ref<Logger> ILoggerFactory::GetDummyLogger()
{
    static Ref<Logger> dummy = CreateRef<Logger>(new Logger());
    return dummy;
}

MINET_END
