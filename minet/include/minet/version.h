#pragma once

#include "minet/common/Base.h"

#define MINET_VERSION_MAJOR 1
#define MINET_VERSION_MINOR 0
#define MINET_VERSION_PATCH 3

#define MINET_TO_VERSION(major, minor, patch) (major * 10000 + minor * 100 + patch)
#define MINET_VERSION                         MINET_TO_VERSION(MINET_VERSION_MAJOR, MINET_VERSION_MINOR, MINET_VERSION_PATCH)

// clang-format off
#define MINET_VERSION_STRING                                                                                           \
    MINET_STRINGIFY(MINET_VERSION_MAJOR)                                                                               \
    "."                                                                                                                \
    MINET_STRINGIFY(MINET_VERSION_MINOR)                                                                               \
    "."                                                                                                                \
    MINET_STRINGIFY(MINET_VERSION_PATCH)
// clang-format on
