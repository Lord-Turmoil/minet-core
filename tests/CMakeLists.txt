# ====================================================================
# minet core unit tests
# ====================================================================

set(minet_tests ParserTest AsyncParserTest WrapperTest ThreadPoolTest)

foreach(test ${minet_tests})
    add_executable(${test} doctest.cpp ${test}.cpp)
    target_include_directories(${test} PRIVATE . $<TARGET_PROPERTY:${MINET_LIB},INCLUDE_DIRECTORIES>)
    target_link_libraries(${test} ${MINET_LIB})
    add_test(NAME ${test} COMMAND ${test})
endforeach(test ${minet_tests})
