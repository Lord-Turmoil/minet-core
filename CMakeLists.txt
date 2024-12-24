# ====================================================================
# Top-level CMakeLists.txt file for Mini-Net Core, a.k.a. minet-core
# ====================================================================

cmake_minimum_required(VERSION 3.16)

include(cmake/utils.cmake)

# --------------------------------------------------------------------
# Project configuration
# --------------------------------------------------------------------

minet_extract_version()

project(minetcore
    VERSION ${MINET_VERSION}
    DESCRIPTION "A mini HTTP server library with C++ mimicking ASP.NET Core"
    LANGUAGES CXX)
set(PROJECT_FULLNAME "minet core")
message(STATUS "Building ${PROJECT_FULLNAME} ${MINET_VERSION}")

option(MINET_ENABLE_ASSERTION "Enable assertions" ON)
option(MINET_BUILD_DEMO "Build demo projects" ON)
option(MINET_BUILD_TEST "Build unit tests" ON)

# Sanitizers, TSan will be ignored if ASan is enabled.
option(MINET_ASAN "Enable AddressSanitizer" OFF)
option(MINET_TSAN "Enable ThreadSanitizer" OFF)

# --------------------------------------------------------------------
# Project options
# --------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

if(NOT DEFINED MINET_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(MINET_MASTER_PROJECT ON)
        message(STATUS "${PROJECT_FULLNAME} is built as the master project")
    else()
        set(MINET_MASTER_PROJECT OFF)
        message(STATUS "${PROJECT_FULLNAME} is built as a sub-project")
    endif()
endif()

# --------------------------------------------------------------------
# Global compiler settings
# --------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)

# --------------------------------------------------------------------
# 3rd-party dependencies
# --------------------------------------------------------------------

add_subdirectory(libs)
message(STATUS "${PROJECT_FULLNAME} dependencies: ${MINET_VENDORS}")

# --------------------------------------------------------------------
# minet core main target
# --------------------------------------------------------------------

add_subdirectory(minet)
target_link_libraries(${MINET_LIB} ${MINET_VENDORS})

# --------------------------------------------------------------------
# Demo and tests
# --------------------------------------------------------------------

if(MINET_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if(MINET_BUILD_TEST)
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "Skipping ${PROJECT_FULLNAME} unit tests")
endif()
