cmake_minimum_required(VERSION 3.14)
project(MyCppFormat LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

find_package(GTest REQUIRED)

include(FetchContent)

# ── tree-sitter 核心库 ──
FetchContent_Declare(
    tree-sitter
    GIT_REPOSITORY git@github.com:tree-sitter/tree-sitter.git
    GIT_TAG        v0.25.3
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  lib
)
FetchContent_MakeAvailable(tree-sitter)

# ── tree-sitter-cpp 语法（仅下载源码，无 CMake 构建系统）──
FetchContent_Declare(
    tree-sitter-cpp
    GIT_REPOSITORY git@github.com:tree-sitter/tree-sitter-cpp.git
    GIT_TAG        v0.23.4
    GIT_PROGRESS   TRUE
)
FetchContent_GetProperties(tree-sitter-cpp)
if(NOT tree-sitter-cpp_POPULATED)
    FetchContent_Populate(tree-sitter-cpp)
endif()

# 使用者需要 tree_sitter/api.h
target_include_directories(tree-sitter INTERFACE
    ${tree-sitter_SOURCE_DIR}/lib/include
)

# 手动创建 tree-sitter-cpp 编译目标
add_library(tree-sitter-cpp STATIC
    ${tree-sitter-cpp_SOURCE_DIR}/src/parser.c
    ${tree-sitter-cpp_SOURCE_DIR}/src/scanner.c
)
# 内联的 tree_sitter 头（alloc.h, parser.h）位于 src/ 下
target_include_directories(tree-sitter-cpp PRIVATE
    ${tree-sitter-cpp_SOURCE_DIR}/src
)
# 使用者需要 tree_sitter/api.h
target_include_directories(tree-sitter-cpp PUBLIC
    ${tree-sitter_SOURCE_DIR}/lib/include
)
target_link_libraries(tree-sitter-cpp PUBLIC tree-sitter)

add_subdirectory(src)
add_subdirectory(tests)
