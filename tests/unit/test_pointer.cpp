#include <gtest/gtest.h>

#include "formatter.hpp"

/// 辅助函数：对输入的 C++ 代码应用指针/引用对齐规则
static std::string fix_pointer(const std::string &input) {
    Formatter fmt;
    return fmt.fixPointerAlignment(input);
}

// ============================================================
// 测试场景1: 基本指针声明 — * 应该紧贴变量名
// 预期: int* a; → int *a;
// ============================================================
TEST(PointerTest, BasicPointerTypeStarAttached) {
    EXPECT_EQ(fix_pointer("int* a;"), "int *a;");
}

// ============================================================
// 测试场景2: * 与变量名之间有空格 — 空格应被移除
// 预期: int * a; → int *a;
// ============================================================
TEST(PointerTest, PointerSpaceBeforeName) {
    EXPECT_EQ(fix_pointer("int * a;"), "int *a;");
}

// ============================================================
// 测试场景3: 多个空格 — 应规范化为单空格
// 预期: int  *  a; → int *a;
// ============================================================
TEST(PointerTest, PointerMultipleSpaces) {
    EXPECT_EQ(fix_pointer("int  *  a;"), "int *a;");
}

// ============================================================
// 测试场景4: 引用声明 — & 应该紧贴变量名
// 预期: double& b; → double &b;
// ============================================================
TEST(PointerTest, BasicRefTypeAttached) {
    EXPECT_EQ(fix_pointer("double& b;"), "double &b;");
}

// ============================================================
// 测试场景5: 引用与变量名之间有空格
// 预期: double & b; → double &b;
// ============================================================
TEST(PointerTest, RefSpaceBeforeName) {
    EXPECT_EQ(fix_pointer("double & b;"), "double &b;");
}

// ============================================================
// 测试场景6: 函数参数中的指针
// 预期: void func(int* a,double* b); → void func(int *a,double *b);
// ============================================================
TEST(PointerTest, FunctionParamPointer) {
    EXPECT_EQ(fix_pointer("void func(int* a,double* b);"),
              "void func(int *a,double *b);");
}

// ============================================================
// 测试场景7: 函数参数中 * 有空格
// 预期: void func(int * a); → void func(int *a);
// ============================================================
TEST(PointerTest, FunctionParamPointerSpace) {
    EXPECT_EQ(fix_pointer("void func(int * a);"), "void func(int *a);");
}

// ============================================================
// 测试场景8: 双重指针 char **argv 已正确不变
// ============================================================
TEST(PointerTest, DoublePointerAlreadyCorrect) {
    EXPECT_EQ(fix_pointer("char **argv;"), "char **argv;");
}

// ============================================================
// 测试场景9: char** argv → char **argv
// ============================================================
TEST(PointerTest, DoublePointerAttachedToType) {
    EXPECT_EQ(fix_pointer("char** argv;"), "char **argv;");
}

// ============================================================
// 测试场景10: 指针的引用 int *&p
// ============================================================
TEST(PointerTest, PointerRef) {
    EXPECT_EQ(fix_pointer("int *& p;"), "int *&p;");
}

// ============================================================
// 测试场景11: 乘法运算不被修改
// 预期: a * b; 保持不变
// ============================================================
TEST(PointerTest, MultiplicationNotModified) {
    EXPECT_EQ(fix_pointer("a * b;"), "a * b;");
}

// ============================================================
// 测试场景12: 解引用不被修改
// 预期: *ptr = 5; 保持不变
// ============================================================
TEST(PointerTest, DereferenceNotModified) {
    EXPECT_EQ(fix_pointer("*ptr = 5;"), "*ptr = 5;");
}

// ============================================================
// 测试场景13: 字符串字面量内的 * 不被修改
// ============================================================
TEST(PointerTest, StringLiteralNotModified) {
    EXPECT_EQ(fix_pointer(R"(const char* msg = "int* a;";)"),
              R"(const char *msg = "int* a;";)");
}

// ============================================================
// 测试场景14: 注释内的 * & 不被修改
// ============================================================
TEST(PointerTest, CommentNotModified) {
    EXPECT_EQ(fix_pointer("// int* a;\nint *b;"), "// int* a;\nint *b;");
}

// ============================================================
// 测试场景15: 预处理器指令不被修改
// ============================================================
TEST(PointerTest, PreprocessorNotModified) {
    EXPECT_EQ(fix_pointer("#define PTR int *\nint* a;"),
              "#define PTR int *\nint *a;");
}

// ============================================================
// 测试场景16: 逗号后有空格分离的指针声明，* 应收紧到变量名
// 预期: int a, * b; → int a,*b;（逗号间距由规范2逗号规则处理）
// ============================================================
TEST(PointerTest, CommaSeparatedDecl) {
    EXPECT_EQ(fix_pointer("int a, * b;"), "int a,*b;");
}
