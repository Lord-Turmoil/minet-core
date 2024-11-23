#pragma once

#include <memory>

#define MINET_BEGIN                                                                                                    \
    namespace minet                                                                                                    \
    {
#define MINET_END }
#define MINET     ::minet::

#define MINET_EXPAND_MACRO(x)    x
#define MINET_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << (x))

#define BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#if defined(DEBUG) || defined(_DEBUG)
#define MINET_DEBUG
#else
#define MINET_RELEASE
#endif

#if defined(MINET_DEBUG)
#define MINET_ENABLE_ASSERT
#endif

#define MINET_TRY(expr)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        auto __r = expr;                                                                                               \
        if (__r < 0)                                                                                                   \
        {                                                                                                              \
            return __r;                                                                                                \
        }                                                                                                              \
    } while (0)

#define MINET_TRY_WITH_ACTION(expr, action)                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        auto __r = expr;                                                                                               \
        if (__r < 0)                                                                                                   \
        {                                                                                                              \
            action;                                                                                                    \
            return __r;                                                                                                \
        }                                                                                                              \
    } while (0)

MINET_BEGIN

template <typename T> using Ref = std::shared_ptr<T>;

template <typename T, typename... Args> constexpr Ref<T> CreateRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T> constexpr Ref<T> CreateRef(T* ptr)
{
    return std::shared_ptr<T>(ptr);
}

MINET_END
