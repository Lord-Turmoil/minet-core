set(LIBRARIES minetcore)
set(MINET_TESTS ParserTest AsyncParserTest WrapperTest ThreadPoolTest)

foreach(test ${MINET_TESTS})
    add_executable(${test} doctest.cpp ${test}.cpp)
    target_include_directories(${test} PRIVATE .)
    target_link_libraries(${test} ${LIBRARIES})
    add_test(NAME ${test} COMMAND ${test})
endforeach(test ${MINET_TESTS})
