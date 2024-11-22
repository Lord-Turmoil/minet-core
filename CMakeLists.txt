cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME "minet-core")
set(BINARY_NAME minetcore)

set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME}
    VERSION 0.1.0
    DESCRIPTION "A mini HTTP server library with C++"
    LANGUAGES CXX)

# dependencies
add_subdirectory(libs)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

# library properties
set(LIBRARY_BASE_PATH "${PROJECT_SOURCE_DIR}")
include_directories("${LIBRARY_BASE_PATH}/include")
file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "${LIBRARY_BASE_PATH}/src/*.cpp")
add_library(${BINARY_NAME} ${SRC_LIST})

# target properties
target_include_directories(${BINARY_NAME} PUBLIC ${INCLUDE_DIRECTORIES})
target_link_libraries(${BINARY_NAME} mioc spdlog)

# install properties
install (
	TARGETS ${BINARY_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

enable_testing()
add_subdirectory(tests)
