set(UNIT_TEST_SOURCES
    test_basic.cpp
    test_bracket.cpp
    test_comma.cpp
    test_keyword.cpp
    test_pointer.cpp
    test_semicolon.cpp
)

add_executable(unit ${UNIT_TEST_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/ts_formatter.cpp
)
target_include_directories(unit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit GTest::gtest GTest::gtest_main pthread
                      tree-sitter tree-sitter-cpp)

add_test(NAME unit COMMAND unit)
