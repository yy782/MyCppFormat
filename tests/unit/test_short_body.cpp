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
// 规范4: 短函数体 — 单行函数体大括号内去除首尾空格
// 仅对 function_definition 内的 compound_statement 生效
// strip_ws_same_line 自动区分单行/跨行：
//   单行 → 去除水平空白；跨行 → 保留原样
// ============================================================

// int f() { return true; } → int f() {return true;}
TEST(ShortBodyTest, SimpleReturn) {
    EXPECT_EQ(fmt("int f() { return true; }"), "int f() {return true;}");
}

// void g() { } → void g() {} — 空大括号
TEST(ShortBodyTest, EmptyFunc) {
    EXPECT_EQ(fmt("void g() { }"), "void g() {}");
}

// int f(){} → int f(){} — 原本就紧凑的无变化
TEST(ShortBodyTest, AlreadyCompact) {
    EXPECT_EQ(fmt("int f(){}"), "int f(){}");
}

// void f() { ; } → void f() {;}
TEST(ShortBodyTest, SingleSemicolon) {
    EXPECT_EQ(fmt("void f() { ; }"), "void f() {;}");
}

// int f() { int a; } → int f() {int a;}
TEST(ShortBodyTest, SingleDeclaration) {
    EXPECT_EQ(fmt("int f() { int a; }"), "int f() {int a;}");
}

// int f() { return a + b; } → int f() {return a + b;}
TEST(ShortBodyTest, ComplexExpression) {
    EXPECT_EQ(fmt("int f() { return a + b; }"), "int f() {return a + b;}");
}

// 多行函数体应不受影响：保留原有换行和缩进
TEST(ShortBodyTest, MultiLinePreserved) {
    EXPECT_EQ(fmt("int f() {\n    int a;\n    return a;\n}"),
              "int f() {\n    int a;\n    return a;\n}");
}

// 跨行：{ 后紧跟换行符的内部内容不被影响
TEST(ShortBodyTest, NewlineImmediatelyAfter) {
    EXPECT_EQ(fmt("int f() {\nreturn a;\n}"),
              "int f() {\nreturn a;\n}");
}

// 含指针返回类型的短函数体：int* 是返回类型不修改，短体规则处理 {}
TEST(ShortBodyTest, WithPointerDeclaration) {
    EXPECT_EQ(fmt("int* f() { *p = val; }"), "int* f() {*p = val;}");
}

// ============================================================
// braced initializer 不应被误判为函数体大括号
// 如 return { 1 }; 中的 { } 是 initializer_list，直接父节点不是
// compound_statement，短函数体规则不应压缩其内部空格
// ============================================================

// return { 1 } — braced initializer 内部空格应保留
TEST(ShortBodyTest, BracedInitReturn) {
    EXPECT_EQ(fmt("int f() { return { 1 }; }"),
              "int f() {return { 1 };}");
}

// return { 1, 2, 3 } — 逗号列表的 braced initializer（逗号规则同时生效）
TEST(ShortBodyTest, BracedInitList) {
    EXPECT_EQ(fmt("int f() { return { 1, 2, 3 }; }"),
              "int f() {return { 1,2,3 };}");
}

// return { { 1 } } — 嵌套 braced initializer
TEST(ShortBodyTest, BracedInitNested) {
    EXPECT_EQ(fmt("int f() { return { { 1 } }; }"),
              "int f() {return { { 1 } };}");
}

// ============================================================
// 不应影响的场景：非函数体内的 { } 保持原样
// ============================================================

// if 体内的 { } 不受短函数体规则影响（分号规则仍会去除 ; 后空格）
TEST(ShortBodyTest, IfBodyNotAffected) {
    EXPECT_EQ(fmt("if (x) { return; }"), "if (x) { return;}");
}

// switch 体内的 { } 不受短函数体规则影响（括号规则去除 () 内空格，分号规则去除 ; 后空格）
TEST(ShortBodyTest, SwitchBodyNotAffected) {
    EXPECT_EQ(fmt("switch (x) { case 1: { break; } }"),
              "switch (x) { case 1: { break;} }");
}

// try-catch 体内的 { } 不受短函数体规则影响
TEST(ShortBodyTest, TryCatchBodyNotAffected) {
    EXPECT_EQ(fmt("try { f(); } catch (...) { }"),
              "try { f();} catch (...) { }");
}
