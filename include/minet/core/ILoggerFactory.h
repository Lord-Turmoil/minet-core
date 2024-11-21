#pragma once

#include "minet/components/Logger.h"

MINET_BEGIN

/**
 * @brief Logger factory interface.
 */
class ILoggerFactory
{
public:
    virtual ~ILoggerFactory() = default;

    virtual Ref<Logger> GetLogger(std::string name);
    virtual Ref<Logger> GetDummyLogger();
};

MINET_END
