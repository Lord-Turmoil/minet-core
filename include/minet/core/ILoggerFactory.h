#pragma once

#include "minet/common/Base.h"

MINET_BEGIN

class Logger;

/**
 * @brief Logger factory interface.
 */
class ILoggerFactory
{
public:
    virtual ~ILoggerFactory() = default;

    virtual Ref<Logger> GetLogger(std::string name) = 0;

    static Ref<Logger> GetDummyLogger();
};

MINET_END
