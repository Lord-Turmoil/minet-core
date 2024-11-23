set(DEMO_BINARY "minet-demo")

add_executable(${DEMO_BINARY} demo.cpp)
target_link_libraries(${DEMO_BINARY} minetcore)
