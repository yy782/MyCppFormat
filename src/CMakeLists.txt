add_executable(codeformat
    main.cpp
    ts_formatter.cpp
    ts_formatter.hpp
)

target_link_libraries(codeformat PRIVATE tree-sitter tree-sitter-cpp)
