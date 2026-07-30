cmake_minimum_required(VERSION 3.14)
project(MyCppFormat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)
