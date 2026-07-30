add_executable(test_pointer test_pointer.cpp
    ${CMAKE_SOURCE_DIR}/src/formatter.cpp
)
target_include_directories(test_pointer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_pointer GTest::gtest GTest::gtest_main pthread)

add_test(NAME test_pointer COMMAND test_pointer)
