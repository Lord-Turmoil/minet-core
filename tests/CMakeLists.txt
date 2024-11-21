aux_source_directory(. TEST_FILES)
add_executable("minet-tests" ${TEST_FILES})

target_include_directories(tests PRIVATE .)

target_link_libraries(tests minetcore)

add_test(NAME TestAll COMMAND tests)
