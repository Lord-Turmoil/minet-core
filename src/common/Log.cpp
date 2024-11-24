#include <algorithm>

#include "minet/common/Log.h"

MINET_BEGIN

LogLevel ParseLogLevel(std::string level)
{
    std::transform(level.begin(), level.end(), level.begin(), ::tolower);
    if (level == "all")
    {
        return LogLevel::All;
    }
    else if (level == "fine")
    {
        return LogLevel::Fine;
    }
    else if (level == "debug")
    {
        return LogLevel::Debug;
    }
    else if (level == "info")
    {
        return LogLevel::Info;
    }
    else if (level == "warning")
    {
        return LogLevel::Warning;
    }
    else if (level == "error")
    {
        return LogLevel::Error;
    }
    else if (level == "critical")
    {
        return LogLevel::Critical;
    }
    else if (level == "disabled")
    {
        return LogLevel::Disabled;
    }
    else
    {
        return LogLevel::Invalid;
    }
}

MINET_END
