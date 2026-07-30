set(UNIT_TEST_SOURCES
    test_pointer.cpp
    # test_comma.cpp      # 待添加：逗号规则测试
    # test_semicolon.cpp  # 待添加：分号规则测试
    # test_bracket.cpp    # 待添加：括号空格规则测试
    # test_keyword.cpp    # 待添加：关键字空格规则测试
    # test_short_body.cpp # 待添加：短函数体规则测试
    # test_formatter.cpp  # 待添加：格式化器整体流程测试
)

add_executable(unit ${UNIT_TEST_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/formatter.cpp
)
target_include_directories(unit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit GTest::gtest GTest::gtest_main pthread)

add_test(NAME unit COMMAND unit)
