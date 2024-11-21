#pragma once

#include <memory>

#define MINET_BEGIN                                                                                                    \
    namespace minet                                                                                                    \
    {
#define MINET_END }
#define MINET     ::minet::

#define EXPAND_MACRO(x)    x
#define STRINGIFY_MACRO(x) #x

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

MINET_BEGIN

template <typename T> using Ref = std::shared_ptr<T>;

template <typename T, typename... Args> constexpr Ref<T> CreateRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

MINET_END
