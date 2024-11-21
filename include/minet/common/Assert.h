#pragma once

#include <filesystem>

#include "minet/common/Base.h"

#ifdef MINET_ENABLE_ASSERT
#define MINET_DEBUG_BREAK() __debugbreak()
#else
#define MINET_DEBUG_BREAK()
#endif

#ifdef MINET_ENABLE_ASSERT

#define _MINET_ASSERT_LOGGER MINET_EXPAND_MACRO(LOG_CRITICAL)

#define _MINET_ASSERT_IMPL(expression, msg, ...)                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expression))                                                                                             \
        {                                                                                                              \
            _MINET_ASSERT_LOGGER(msg, __VA_ARGS__);                                                                    \
            MINET_DEBUG_BREAK();                                                                                       \
        }                                                                                                              \
    } while (0)

#define _MINET_ASSERT_WITH_MSG(expression, ...)                                                                        \
    _MINET_ASSERT_IMPL(expression, "Assertion '{0}' failed at {1}:{2} with message: {3}",                              \
                       MINET_STRINGIFY_MACRO(expression), std::filesystem::path(__FILE__).filename().string(),         \
                       __LINE__, __VA_ARGS__)

#define _MINET_ASSERT_NO_MSG(expression)                                                                               \
    _MINET_ASSERT_IMPL(expression, "Assertion '{0}' failed at {1}:{2}", MINET_STRINGIFY_MACRO(expression),             \
                       std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define _MINET_ASSERT_MACRO_NAME(_1, _2, macro, ...) macro
#define _MINET_ASSERT_MACRO(...)                                                                                       \
    MINET_EXPAND_MACRO(_MINET_ASSERT_MACRO_NAME(__VA_ARGS__, _MINET_ASSERT_WITH_MSG, _MINET_ASSERT_NO_MSG))

// Only support one string message (expression, message) or (expression)
#define MINET_ASSERT(...) MINET_EXPAND_MACRO(_MINET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else
#define MINET_ASSERT(...)
#endif
