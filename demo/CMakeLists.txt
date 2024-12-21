# ====================================================================
# minet core demo server
# ====================================================================

project(minet-demo)

add_executable(${PROJECT_NAME} demo.cpp)

target_link_libraries(${PROJECT_NAME} ${MINET_LIB})
