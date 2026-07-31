add_executable(MyCppFormat
    main.cpp
    ts_formatter.cpp
    ts_formatter.hpp
)

target_link_libraries(MyCppFormat PRIVATE tree-sitter tree-sitter-cpp)
