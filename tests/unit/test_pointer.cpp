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

// std::vector<int*>& → std::vector<int *>&
TEST(PointerTest, InTemplate) {
    EXPECT_EQ(fmt("std::vector<int*> v;"), "std::vector<int *> v;");
}

// a * b（乘法）不应被修改
TEST(PointerTest, NotMultiplication) {
    EXPECT_EQ(fmt("int c = a * b;"), "int c = a * b;");
}

// a & b（按位与）不应被修改
TEST(PointerTest, NotBitwiseAnd) {
    EXPECT_EQ(fmt("int c = a & b;"), "int c = a & b;");
}
