#include "ts_formatter.hpp"

#include <gtest/gtest.h>

// ============================================================
// 测试辅助：创建格式化器并执行格式化
// ============================================================
static std::string fmt(const std::string &source) {
    TsFormatter f;
    return f.format(source);
}

// ============================================================
// 恒等变换测试（规则不触发的输入应保持原样）
// ============================================================

// 测试场景1: 无规则触发时应保持原样
TEST(TsFormatterTest, Identity_SimpleFunc) {
    // 注意：`; }` 中的分号规则会触发，所以最终输出是 `;}` 
    EXPECT_EQ(fmt("int f() { return 0; }"), "int f() { return 0;}");
}

// 测试场景2: 字符串字面量 — 内容绝对不变
TEST(TsFormatterTest, Identity_StringLiteral) {
    // 字符串内部的格式标记不应受影响
    // char* → char * 是指针规则正常触发
    EXPECT_EQ(fmt("const char* s = \"int* a; , ; ( ) if();\";"),
              "const char *s = \"int* a; , ; ( ) if();\";");
}

// 测试场景3: 行注释 — 注释内容不变
TEST(TsFormatterTest, Identity_LineComment) {
    // 分号规则会触发：`a; //` → `a;//`（分号后空格移除）
    EXPECT_EQ(fmt("int a; // int* b, c; if()"),
              "int a;// int* b, c; if()");
}

// 测试场景4: 块注释 — 内容绝对不变
TEST(TsFormatterTest, Identity_BlockComment) {
    EXPECT_EQ(
        fmt("/* int* a;\n * b; if() */"),
        "/* int* a;\n * b; if() */");
}

// 测试场景5: 预处理器指令 — 内容绝对不变
TEST(TsFormatterTest, Identity_Preprocessor) {
    EXPECT_EQ(fmt("#include <iostream>"), "#include <iostream>");
    EXPECT_EQ(fmt("#define FOO(x) ((x) + 1)"), "#define FOO(x) ((x) + 1)");
}

// 测试场景6: 空输入
TEST(TsFormatterTest, Identity_EmptySource) {
    EXPECT_EQ(fmt(""), "");
}

// 测试场景7: 纯空白
TEST(TsFormatterTest, Identity_WhitespaceOnly) {
    EXPECT_EQ(fmt("\n\n"), "\n\n");
    EXPECT_EQ(fmt("  \n\t\n"), "  \n\t\n");
}

// ============================================================
// 规范1: 指针/引用对齐
// ============================================================

// int* a → int *a
TEST(TsFormatterTest, Pointer_BasicInt) {
    EXPECT_EQ(fmt("int* a;"), "int *a;");
}

// double& b → double &b
TEST(TsFormatterTest, Pointer_BasicRef) {
    EXPECT_EQ(fmt("double& b;"), "double &b;");
}

// int * a → int *a（移除多余空格）
TEST(TsFormatterTest, Pointer_RemoveExtraSpace) {
    EXPECT_EQ(fmt("int * a;"), "int *a;");
}

// char** argv → char **argv
TEST(TsFormatterTest, Pointer_DoublePointer) {
    EXPECT_EQ(fmt("char** argv;"), "char **argv;");
}

// int*& p → int *&p
TEST(TsFormatterTest, Pointer_PointerRefCombo) {
    EXPECT_EQ(fmt("int*& p;"), "int *&p;");
}

// void func(int* a, double& b);
// 指针规则 + 逗号规则同时触发
TEST(TsFormatterTest, Pointer_InFunctionParams) {
    EXPECT_EQ(fmt("void func(int* a, double& b);"),
              "void func(int *a,double &b);");
}

// const char* msg → const char *msg
TEST(TsFormatterTest, Pointer_ConstChar) {
    EXPECT_EQ(fmt("const char* msg;"), "const char *msg;");
}

// std::vector<int*>& → std::vector<int *>&
TEST(TsFormatterTest, Pointer_InTemplate) {
    EXPECT_EQ(fmt("std::vector<int*> v;"), "std::vector<int *> v;");
}

// a * b（乘法）不应被修改
TEST(TsFormatterTest, Pointer_NotMultiplication) {
    EXPECT_EQ(fmt("int c = a * b;"), "int c = a * b;");
}

// a & b（按位与）不应被修改
TEST(TsFormatterTest, Pointer_NotBitwiseAnd) {
    EXPECT_EQ(fmt("int c = a & b;"), "int c = a & b;");
}

// ============================================================
// 规范2: 逗号分隔 — 逗号后不加空格
// ============================================================

TEST(TsFormatterTest, Comma_FuncArgs) {
    EXPECT_EQ(fmt("func(a, b, c)"), "func(a,b,c)");
}

TEST(TsFormatterTest, Comma_VarDecl) {
    EXPECT_EQ(fmt("int a, b, c;"), "int a,b,c;");
}

TEST(TsFormatterTest, Comma_MultipleSpaces) {
    EXPECT_EQ(fmt("func(a,   b,     c)"), "func(a,b,c)");
}

// ============================================================
// 规范3: 分号分隔 — 分号后不加空格
// ============================================================

TEST(TsFormatterTest, Semicolon_ForLoop) {
    EXPECT_EQ(fmt("for (int i = 0; i < 10; ++i)"),
              "for (int i = 0;i < 10;++i)");
}

TEST(TsFormatterTest, Semicolon_MultipleSpaces) {
    EXPECT_EQ(fmt("int a;   int b;"), "int a;int b;");
}

// ============================================================
// 规范5: 括号内空格 — 去除括号内首尾空格
// ============================================================

TEST(TsFormatterTest, Bracket_If) {
    EXPECT_EQ(fmt("if ( x > 0 )"), "if (x > 0)");
}

TEST(TsFormatterTest, Bracket_For) {
    EXPECT_EQ(fmt("for ( int i = 0; i < 10; ++i )"),
              "for (int i = 0;i < 10;++i)");
}

TEST(TsFormatterTest, Bracket_While) {
    EXPECT_EQ(fmt("while ( x < 5 )"), "while (x < 5)");
}

// ============================================================
// 规范6: 关键字空格 — 关键字与 '(' 间保留一个空格
// ============================================================

TEST(TsFormatterTest, Keyword_If) {
    EXPECT_EQ(fmt("if(condition)"), "if (condition)");
}

TEST(TsFormatterTest, Keyword_For) {
    EXPECT_EQ(fmt("for(int i=0;i<10;++i)"),
              "for (int i=0;i<10;++i)");
}

TEST(TsFormatterTest, Keyword_While) {
    EXPECT_EQ(fmt("while(x<5)"), "while (x<5)");
}

TEST(TsFormatterTest, Keyword_Switch) {
    // switch 需要完整上下文才能被 tree-sitter 正确解析
    EXPECT_EQ(fmt("switch(val){default:;}"), "switch (val){default:;}");
}

TEST(TsFormatterTest, Keyword_Catch) {
    // catch 需要 try-catch 完整上下文才能被 tree-sitter 正确识别
    EXPECT_EQ(fmt("try{}catch(std::exception& e){}"),
              "try{}catch (std::exception &e){}");
}

// 非关键字后跟 '(' 不应受影响（如函数调用）
TEST(TsFormatterTest, Keyword_NotFunctionCall) {
    EXPECT_EQ(fmt("func(arg)"), "func(arg)");
}

// ============================================================
// ERROR 节点处理 — 语法错误区域保持原样
// ============================================================

// 不完整的代码不应崩溃
TEST(TsFormatterTest, Error_Recovery) {
    const char *input = "int x = ;";
    EXPECT_EQ(fmt(input), input);
}

// 模板语法过于复杂时的降级处理
TEST(TsFormatterTest, Error_ComplexTemplate) {
    // 部分复杂模板可能产生 ERROR 节点，应保持原样
    const char *input = "template<template<typename>> class A {};";
    // 只要能格式化不崩溃即可，具体输出取决于解析结果
    EXPECT_NO_THROW(fmt(input));
}
