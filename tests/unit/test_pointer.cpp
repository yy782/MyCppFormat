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
// 规范1: 指针/引用对齐
// ============================================================

// int* a → int *a
TEST(PointerTest, BasicInt) {
    EXPECT_EQ(fmt("int* a;"), "int *a;");
}

// double& b → double &b
TEST(PointerTest, BasicRef) {
    EXPECT_EQ(fmt("double& b;"), "double &b;");
}

// int * a → int *a（移除多余空格）
TEST(PointerTest, RemoveExtraSpace) {
    EXPECT_EQ(fmt("int * a;"), "int *a;");
}

// char** argv → char **argv
TEST(PointerTest, DoublePointer) {
    EXPECT_EQ(fmt("char** argv;"), "char **argv;");
}

// int*& p → int *&p
TEST(PointerTest, PointerRefCombo) {
    EXPECT_EQ(fmt("int*& p;"), "int *&p;");
}

// void func(int* a, double& b);
// 指针规则 + 逗号规则同时触发
TEST(PointerTest, InFunctionParams) {
    EXPECT_EQ(fmt("void func(int* a, double& b);"),
              "void func(int *a,double &b);");
}

// const char* msg → const char *msg
TEST(PointerTest, ConstChar) {
    EXPECT_EQ(fmt("const char* msg;"), "const char *msg;");
}

// 模板参数中的 int* 不是变量声明（没有变量名），不应用指针规则
TEST(PointerTest, InTemplate) {
    EXPECT_EQ(fmt("std::vector<int*> v;"), "std::vector<int*> v;");
}

// a * b（乘法）不应被修改
TEST(PointerTest, NotMultiplication) {
    EXPECT_EQ(fmt("int c = a * b;"), "int c = a * b;");
}

// a & b（按位与）不应被修改
TEST(PointerTest, NotBitwiseAnd) {
    EXPECT_EQ(fmt("int c = a & b;"), "int c = a & b;");
}

// , * → ,*  逗号规则优先于指针前置空格（* 在 pointer_declarator 内）
// 输入 char *a, *b; → 逗号后不应因为指针规则而添加空格
TEST(PointerTest, CommaBeforePointerDecl) {
    EXPECT_EQ(fmt("char *a, *b;"), "char *a,*b;");
}

// , * → ,*  多个逗号分隔的指针声明，均不添加空格
TEST(PointerTest, CommaBeforeMultiPointerDecl) {
    EXPECT_EQ(fmt("int *p, *q, *r;"), "int *p,*q,*r;");
}

// ( * → (*  括号规则优先于指针前置空格（* 在抽象指针声明器内）
// 输入 int ( *p ); → 左括号后不应因为指针规则而添加空格
TEST(PointerTest, ParenBeforePointerDecl) {
    EXPECT_EQ(fmt("int ( *p );"), "int (*p);");
}

// ============================================================
// 跨行指针声明 — 不应被折叠为单行
// ============================================================

// int\n*p; → 保留换行，不折叠为 int *p;
TEST(PointerTest, MultiLinePointerPreservesNewline) {
    EXPECT_EQ(fmt("int\n*p;"), "int\n*p;");
}

// int\n&r; → 保留换行，不折叠为 int &r;
TEST(PointerTest, MultiLineRefPreservesNewline) {
    EXPECT_EQ(fmt("int\n&r;"), "int\n&r;");
}

// char\n**argv; → 保留换行和缩进
TEST(PointerTest, MultiLineDoublePointer) {
    EXPECT_EQ(fmt("char\n**argv;"), "char\n**argv;");
}

// int\n*&p; → 保留换行
TEST(PointerTest, MultiLinePointerRefCombo) {
    EXPECT_EQ(fmt("int\n*&p;"), "int\n*&p;");
}
