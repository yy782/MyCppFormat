set(UNIT_TEST_SOURCES
    test_pointer.cpp
    test_comma.cpp
    test_semicolon.cpp
    test_bracket.cpp
    test_keyword.cpp
    test_short_body.cpp
    # test_formatter.cpp  # 待添加：格式化器整体流程测试
)

add_executable(unit ${UNIT_TEST_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/formatter.cpp
)
target_include_directories(unit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit GTest::gtest GTest::gtest_main pthread)

add_test(NAME unit COMMAND unit)
