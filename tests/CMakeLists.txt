# ====================================================================
# minet core unit tests
# ====================================================================

set(MINET_TESTS ParserTest AsyncParserTest WrapperTest ThreadPoolTest)

foreach(test ${MINET_TESTS})
    add_executable(${test} doctest.cpp ${test}.cpp)
    target_include_directories(${test} PRIVATE . $<TARGET_PROPERTY:${MINET_LIB},INCLUDE_DIRECTORIES>)
    target_link_libraries(${test} ${MINET_LIB})
    add_test(NAME ${test} COMMAND ${test})
endforeach(test ${MINET_TESTS})
