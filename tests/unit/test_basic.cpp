#include "ts_formatter.hpp"

#include <gtest/gtest.h>

// ============================================================
// 测试辅助
// ============================================================
static std::string fmt(const std::string &source) {
    TsFormatter f;
    return f.format(source);
}

// ============================================================
// 恒等变换测试（规则不触发的输入应保持原样）
// ============================================================

// 字符串字面量 — 内容绝对不变（指针规则正常触发在变量声明处）
TEST(BasicTest, IdentityStringLiteral) {
    EXPECT_EQ(fmt("const char* s = \"int* a; , ; ( ) if();\";"),
              "const char *s = \"int* a; , ; ( ) if();\";");
}

// 块注释 — 内容绝对不变
TEST(BasicTest, IdentityBlockComment) {
    EXPECT_EQ(
        fmt("/* int* a;\n * b; if() */"),
        "/* int* a;\n * b; if() */");
}

// 预处理器指令 — 内容绝对不变
TEST(BasicTest, IdentityPreprocessor) {
    EXPECT_EQ(fmt("#include <iostream>"), "#include <iostream>");
    EXPECT_EQ(fmt("#define FOO(x) ((x) + 1)"), "#define FOO(x) ((x) + 1)");
}

// 空输入
TEST(BasicTest, IdentityEmptySource) {
    EXPECT_EQ(fmt(""), "");
}

// 纯空白
TEST(BasicTest, IdentityWhitespaceOnly) {
    EXPECT_EQ(fmt("\n\n"), "\n\n");
    EXPECT_EQ(fmt("  \n\t\n"), "  \n\t\n");
}

// ============================================================
// ERROR 节点处理 — 语法错误区域保持原样
// ============================================================

// 不完整的代码不应崩溃
TEST(BasicTest, ErrorRecovery) {
    const char *input = "int x = ;";
    EXPECT_EQ(fmt(input), input);
}

// 模板语法过于复杂时的降级处理
TEST(BasicTest, ErrorComplexTemplate) {
    const char *input = "template<template<typename>> class A {};";
    EXPECT_NO_THROW(fmt(input));
}
