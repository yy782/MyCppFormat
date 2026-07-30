#include <gtest/gtest.h>

#include "formatter.hpp"

/// 辅助函数：对输入的 C++ 代码应用逗号规则
static std::string fix_comma(const std::string &input) {
    Formatter fmt;
    return fmt.fixCommaSpacing(input);
}

// ============================================================
// 测试场景1: 函数调用 — 逗号后空格应被移除
// 预期: func(a, b, c) → func(a,b,c)
// ============================================================
TEST(CommaTest, FunctionCall) {
    EXPECT_EQ(fix_comma("func(a, b, c)"), "func(a,b,c)");
}

// ============================================================
// 测试场景2: 声明语句 — 逗号后空格应被移除
// 预期: int a, b, c; → int a,b,c;
// ============================================================
TEST(CommaTest, Declaration) {
    EXPECT_EQ(fix_comma("int a, b, c;"), "int a,b,c;");
}

// ============================================================
// 测试场景3: 多个空格 — 逗号后所有空格都应移除
// 预期: func(a,  b,   c) → func(a,b,c)
// ============================================================
TEST(CommaTest, MultipleSpaces) {
    EXPECT_EQ(fix_comma("func(a,  b,   c)"), "func(a,b,c)");
}

// ============================================================
// 测试场景4: 模板参数 — 逗号后空格应被移除
// 预期: std::map<int, std::string> → std::map<int,std::string>
// ============================================================
TEST(CommaTest, TemplateArgs) {
    EXPECT_EQ(fix_comma("std::map<int, std::string>"),
              "std::map<int,std::string>");
}

// ============================================================
// 测试场景5: 初始化列表 — 逗号后空格应被移除
// 预期: {1, 2, 3} → {1,2,3}
// ============================================================
TEST(CommaTest, InitializerList) {
    EXPECT_EQ(fix_comma("{1, 2, 3}"), "{1,2,3}");
}

// ============================================================
// 测试场景6: 逗号后无空格 — 保持不变
// 预期: func(a,b,c) → func(a,b,c)
// ============================================================
TEST(CommaTest, AlreadyCorrect) {
    EXPECT_EQ(fix_comma("func(a,b,c)"), "func(a,b,c)");
}

// ============================================================
// 测试场景7: 字符串字面量内的逗号不被修改
// 预期: "hello, world" → "hello, world"
// ============================================================
TEST(CommaTest, StringLiteralNotModified) {
    EXPECT_EQ(fix_comma(R"("hello, world")"), R"("hello, world")");
}

// ============================================================
// 测试场景8: 行注释内的逗号不被修改
// 预期: // a, b, c 保持不变
// ============================================================
TEST(CommaTest, LineCommentNotModified) {
    EXPECT_EQ(fix_comma("// a, b, c\nint x, y;"), "// a, b, c\nint x,y;");
}

// ============================================================
// 测试场景9: 块注释内的逗号不被修改
// 预期: /* a, b, c */ 保持不变
// ============================================================
TEST(CommaTest, BlockCommentNotModified) {
    EXPECT_EQ(fix_comma("/* a, b, c */\nint x, y;"), "/* a, b, c */\nint x,y;");
}

// ============================================================
// 测试场景10: 预处理器指令内的逗号不被修改
// 预期: #define FOO(a, b) 保持不变
// ============================================================
TEST(CommaTest, PreprocessorNotModified) {
    EXPECT_EQ(fix_comma("#define FOO(a, b)\nint x, y;"),
              "#define FOO(a, b)\nint x,y;");
}

// ============================================================
// 测试场景11: 制表符 t 也应被移除
// 预期: func(a,\tb) → func(a,b)
// ============================================================
TEST(CommaTest, TabAfterComma) {
    EXPECT_EQ(fix_comma("func(a,\tb)"), "func(a,b)");
}

// ============================================================
// 测试场景12: 逗号出现在字符字面量中不被修改
// 预期: ',' → ','
// ============================================================
TEST(CommaTest, CharLiteralNotModified) {
    EXPECT_EQ(fix_comma("','"), "','");
}
