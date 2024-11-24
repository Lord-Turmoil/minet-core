#include <algorithm>

#include "minet/common/Log.h"

MINET_BEGIN

LogLevel ParseLogLevel(std::string level)
{
    std::transform(level.begin(), level.end(), level.begin(), tolower);
    if (level == "all")
    {
        return LogLevel::All;
    }
    if (level == "fine")
    {
        return LogLevel::Fine;
    }
    if (level == "debug")
    {
        return LogLevel::Debug;
    }
    if (level == "info")
    {
        return LogLevel::Info;
    }
    if (level == "warning")
    {
        return LogLevel::Warning;
    }
    if (level == "error")
    {
        return LogLevel::Error;
    }
    if (level == "critical")
    {
        return LogLevel::Critical;
    }
    if (level == "disabled")
    {
        return LogLevel::Disabled;
    }
    return LogLevel::Invalid;
}

MINET_END
