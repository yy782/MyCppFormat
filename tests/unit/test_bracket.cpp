#include <gtest/gtest.h>
#include "formatter.hpp"

// ============================================================
// 每个 TEST 独立调用 fixBracketSpace，该函数内部执行
// protect → apply → restore 完整流程
// ============================================================

static std::string fix_bracket(const std::string &s) {
    Formatter fmt;
    return fmt.fixBracketSpace(s);
}

// ============================================================
// 测试场景1: if 语句括号内前后空格 — 应去除
// 预期: if ( x ) → if (x)
// ============================================================
TEST(BracketTest, IfWithInnerSpaces) {
    EXPECT_EQ(fix_bracket("if ( x )"), "if (x)");
}

// ============================================================
// 测试场景2: if 语句括号内仅前空格
// 预期: if ( x) → if (x)
// ============================================================
TEST(BracketTest, IfWithLeadingSpace) {
    EXPECT_EQ(fix_bracket("if ( x)"), "if (x)");
}

// ============================================================
// 测试场景3: if 语句括号内仅后空格
// 预期: if (x ) → if (x)
// ============================================================
TEST(BracketTest, IfWithTrailingSpace) {
    EXPECT_EQ(fix_bracket("if (x )"), "if (x)");
}

// ============================================================
// 测试场景4: if 语句括号内无多余空格 — 保持不变
// 预期: if (x) → if (x)
// ============================================================
TEST(BracketTest, IfNoInnerSpaces) {
    EXPECT_EQ(fix_bracket("if (x)"), "if (x)");
}

// ============================================================
// 测试场景5: for 循环括号内空格
// 预期: for ( int i = 0 ; i < n ; i++ ) → for (int i = 0 ; i < n ; i++)
// ============================================================
TEST(BracketTest, ForWithInnerSpaces) {
    EXPECT_EQ(fix_bracket("for ( int i = 0 ; i < n ; i++ )"),
              "for (int i = 0 ; i < n ; i++)");
}

// ============================================================
// 测试场景6: while 循环括号内空格
// 预期: while ( cond ) → while (cond)
// ============================================================
TEST(BracketTest, WhileWithInnerSpaces) {
    EXPECT_EQ(fix_bracket("while ( cond )"), "while (cond)");
}

// ============================================================
// 测试场景7: 多层括号嵌套
// 预期: if ( (a) ) → if ((a))   — 每层括号独立去空格
// ============================================================
TEST(BracketTest, NestedParentheses) {
    EXPECT_EQ(fix_bracket("if ( ( a ) )"), "if ((a))");
}

// ============================================================
// 测试场景8: 括号内仅空白 — 尽量保留
// 预期: if ( ) → if ()
// ============================================================
TEST(BracketTest, EmptyParentheses) {
    EXPECT_EQ(fix_bracket("if ( )"), "if ()");
}

// ============================================================
// 测试场景9: 字符串和字符内部的括号不受影响
// 预期: " ( 和 ) " 保持不变
// ============================================================
TEST(BracketTest, StringLiteralNotModified) {
    EXPECT_EQ(fix_bracket("if (\" ( ) \")"), "if (\" ( ) \")");
}

// ============================================================
// 测试场景10: 多行代码混合
// 预期: 括号去空格 + 外部代码不变
// ============================================================
TEST(BracketTest, MultiLineMixed) {
    const char *input =
        "if ( x > 0 ) {\n"
        "    return;\n"
        "}\n"
        "for ( int i = 0 ; i < n ; i++ ) {\n"
        "    sum += i;\n"
        "}\n";
    const char *expected =
        "if (x > 0) {\n"
        "    return;\n"
        "}\n"
        "for (int i = 0 ; i < n ; i++) {\n"
        "    sum += i;\n"
        "}\n";
    EXPECT_EQ(fix_bracket(input), expected);
}
