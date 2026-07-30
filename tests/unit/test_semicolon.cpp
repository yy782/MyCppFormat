#include <gtest/gtest.h>

#include "formatter.hpp"

/// 辅助函数：对输入的 C++ 代码应用分号规则
static std::string fix_semicolon(const std::string &input) {
    Formatter fmt;
    return fmt.fixSemicolonSpacing(input);
}

// ============================================================
// 测试场景1: for 循环 — 分号后空格应被移除
// 预期: for (int i = 0; i < 10; ++i) → for (int i = 0;i < 10;++i)
// ============================================================
TEST(SemicolonTest, ForLoop) {
    EXPECT_EQ(fix_semicolon("for (int i = 0; i < 10; ++i)"),
              "for (int i = 0;i < 10;++i)");
}

// ============================================================
// 测试场景2: 多条语句 — 分号后空格应被移除
// 预期: a; b; c; → a;b;c;
// ============================================================
TEST(SemicolonTest, MultipleStatements) {
    EXPECT_EQ(fix_semicolon("a; b; c;"), "a;b;c;");
}

// ============================================================
// 测试场景3: 制表符 — 分号后 tab 应被移除
// 预期: a;\tb; → a;b;
// ============================================================
TEST(SemicolonTest, TabAfterSemicolon) {
    EXPECT_EQ(fix_semicolon("a;\tb;"), "a;b;");
}

// ============================================================
// 测试场景4: 已合规 — 分号后无空格则不变
// 预期: a;b; → a;b;
// ============================================================
TEST(SemicolonTest, AlreadyCorrect) {
    EXPECT_EQ(fix_semicolon("a;b;"), "a;b;");
}

// ============================================================
// 测试场景5: 空 for — 连续分号无空格则不变
// 预期: for (;;) → for (;;)
// ============================================================
TEST(SemicolonTest, EmptyForLoop) {
    EXPECT_EQ(fix_semicolon("for (;;)"), "for (;;)");
}

// ============================================================
// 测试场景6: 字符串字面量内的分号不被修改
// 预期: "; " 保持不变
// ============================================================
TEST(SemicolonTest, StringLiteralNotModified) {
    EXPECT_EQ(fix_semicolon(R"("; ")"), R"("; ")");
}

// ============================================================
// 测试场景7: 字符字面量内的分号不被修改
// 预期: ';' → ';'
// ============================================================
TEST(SemicolonTest, CharLiteralNotModified) {
    EXPECT_EQ(fix_semicolon("';'"), "';'");
}

// ============================================================
// 测试场景8: 行注释内的分号不被修改
// 预期: // a; b 保持不变
// ============================================================
TEST(SemicolonTest, LineCommentNotModified) {
    EXPECT_EQ(fix_semicolon("// a; b\nint x; y;"),
              "// a; b\nint x;y;");
}

// ============================================================
// 测试场景9: 预处理器指令内的分号不被修改
// 预期: #define FOO(a; b) 保持不变
// ============================================================
TEST(SemicolonTest, PreprocessorNotModified) {
    EXPECT_EQ(fix_semicolon("#define FOO(a; b)\nint x; y;"),
              "#define FOO(a; b)\nint x;y;");
}

// ============================================================
// 测试场景10: 多行宏定义续行的分号不被修改
// 预期: #define BAR(x) \\\n  x; 保持不变
// ============================================================
TEST(SemicolonTest, MultiLineMacroNotModified) {
    const char *input =
        "#define BAR(x) \\\n"
        "  x;\n"
        "int a; b;";
    const char *expected =
        "#define BAR(x) \\\n"
        "  x;\n"
        "int a;b;";
    EXPECT_EQ(fix_semicolon(input), expected);
}

// ============================================================
// 测试场景11: 语句末尾分号无后续空格则不变
// 预期: return 0; → return 0;
// ============================================================
TEST(SemicolonTest, StatementEndNoTrailingSpace) {
    EXPECT_EQ(fix_semicolon("return 0;"), "return 0;");
}

// ============================================================
// 测试场景12: 多个空格 — 分号后所有空白都应移除
// 预期: a;  b;   c; → a;b;c;
// ============================================================
TEST(SemicolonTest, MultipleSpaces) {
    EXPECT_EQ(fix_semicolon("a;  b;   c;"), "a;b;c;");
}
