set(TEST_BINARY "minet-tests")

aux_source_directory(. TEST_FILES)
add_executable(${TEST_BINARY} ${TEST_FILES})
target_include_directories(${TEST_BINARY} PRIVATE .)
target_link_libraries(${TEST_BINARY} minetcore)

add_test(NAME TestAll COMMAND tests)
