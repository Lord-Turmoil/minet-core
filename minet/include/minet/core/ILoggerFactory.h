/**
 * @author Tony S.
 * @details Interface for logger factory.
 */

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

    virtual Ref<Logger> GetLogger(const std::string& name) = 0;

    static Ref<Logger> GetDummyLogger();
};

MINET_END
