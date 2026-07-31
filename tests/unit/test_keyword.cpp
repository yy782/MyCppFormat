#include "formatter.hpp"
#include <gtest/gtest.h>

// 辅助函数：仅调用关键字空格规则
static std::string fix_keyword(const std::string &src) {
    return Formatter().fixKeywordSpace(src);
}

// ============================================================
// 测试场景1: if 关键字 — if( → if (
// 预期: if(x) → if (x)
// ============================================================
TEST(KeywordTest, IfWithoutSpace) {
    EXPECT_EQ(fix_keyword("if(x)"), "if (x)");
}

// ============================================================
// 测试场景2: if 关键字 — 已有空格不变
// 预期: if (x) → if (x)（不重复加空格）
// ============================================================
TEST(KeywordTest, IfWithSpace) {
    EXPECT_EQ(fix_keyword("if (x)"), "if (x)");
}

// ============================================================
// 测试场景3: if 关键字 — 多余空格归一化为单空格
// 预期: if   (x) → if (x)
// ============================================================
TEST(KeywordTest, IfMultipleSpaces) {
    EXPECT_EQ(fix_keyword("if   (x)"), "if (x)");
}

// ============================================================
// 测试场景4: for 关键字 — for( → for (
// 预期: for(int i=0;i<n;++i) → for (int i=0;i<n;++i)
// ============================================================
TEST(KeywordTest, ForWithoutSpace) {
    EXPECT_EQ(fix_keyword("for(int i=0;i<n;++i)"),
              "for (int i=0;i<n;++i)");
}

// ============================================================
// 测试场景5: while 关键字
// 预期: while(true) → while (true)
// ============================================================
TEST(KeywordTest, WhileWithoutSpace) {
    EXPECT_EQ(fix_keyword("while(true)"), "while (true)");
}

// ============================================================
// 测试场景6: switch 关键字
// 预期: switch(x) → switch (x)
// ============================================================
TEST(KeywordTest, SwitchWithoutSpace) {
    EXPECT_EQ(fix_keyword("switch(x)"), "switch (x)");
}

// ============================================================
// 测试场景7: catch 关键字
// 预期: catch(exception& e) → catch (exception& e)
// ============================================================
TEST(KeywordTest, CatchWithoutSpace) {
    EXPECT_EQ(fix_keyword("catch(exception& e)"),
              "catch (exception& e)");
}

// ============================================================
// 测试场景8: 函数名后不应加空格 — func( 保持
// 预期: foo(x) → foo(x)（函数调用不变）
// ============================================================
TEST(KeywordTest, FunctionCallNotModified) {
    EXPECT_EQ(fix_keyword("foo(x)"), "foo(x)");
}

// ============================================================
// 测试场景9: else if — if 前面有 else 也能匹配
// 预期: else if(x) → else if (x)
// ============================================================
TEST(KeywordTest, ElseIf) {
    EXPECT_EQ(fix_keyword("else if(x)"), "else if (x)");
}

// ============================================================
// 测试场景10: 嵌套括号 — if((x))
// 预期: if((x)) → if ((x))
// ============================================================
TEST(KeywordTest, NestedParentheses) {
    EXPECT_EQ(fix_keyword("if((x))"), "if ((x))");
}

// ============================================================
// 测试场景11: 字符串内关键字不受影响
// 预期: "while(true)" 保持不变
// ============================================================
TEST(KeywordTest, KeywordInStringNotModified) {
    EXPECT_EQ(fix_keyword("const char *s = \"while(true)\";"),
              "const char *s = \"while(true)\";");
}

// ============================================================
// 测试场景12: do-while 循环
// 预期: do{}while(true); → do{}while (true);
// ============================================================
TEST(KeywordTest, DoWhile) {
    EXPECT_EQ(fix_keyword("do{}while(true);"), "do{}while (true);");
}
