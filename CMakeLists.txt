cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME "minet-core")
set(BINARY_NAME minetcore)

set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME}
    VERSION 1.0.1
    DESCRIPTION "A mini HTTP server library with C++"
    LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# dependencies
add_subdirectory(libs)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    # ASan and TSan are incompatible, so we can't use them together.
    # And because we use std::shared_ptr, it is most likely that there is no need for ASan.
    # However, TSan is somehow too strict, enable it only temporarily.
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread,undefined")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
endif()

# strict warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# optimization flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# library properties
set(LIBRARY_BASE_PATH "${PROJECT_SOURCE_DIR}")
set(PUBLIC_INCLUDE_DIRECTORIES "${LIBRARY_BASE_PATH}/include")
set(PRIVATE_INCLUDE_DIRECTOREIS "${LIBRARY_BASE_PATH}/src")
include_directories(${PUBLIC_INCLUDE_DIRECTORIES} ${PRIVATE_INCLUDE_DIRECTOREIS})
file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "${LIBRARY_BASE_PATH}/src/*.cpp")
add_library(${BINARY_NAME} ${SRC_LIST})

# target properties
target_include_directories(${BINARY_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(${BINARY_NAME} mioc spdlog nlohmann_json)

# install properties
install (
	TARGETS ${BINARY_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

enable_testing()
add_subdirectory(tests)
add_subdirectory(demo)
