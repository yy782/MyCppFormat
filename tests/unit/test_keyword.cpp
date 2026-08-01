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
// 规范6: 关键字空格 — 关键字与 '(' 间保留一个空格
// ============================================================

TEST(KeywordTest, If) {
    EXPECT_EQ(fmt("if(condition)"), "if (condition)");
}

TEST(KeywordTest, For) {
    EXPECT_EQ(fmt("for(int i=0;i<10;++i)"),
              "for (int i=0;i<10;++i)");
}

TEST(KeywordTest, While) {
    EXPECT_EQ(fmt("while(x<5)"), "while (x<5)");
}

TEST(KeywordTest, Switch) {
    // switch 需要完整上下文才能被 tree-sitter 正确解析
    EXPECT_EQ(fmt("switch(val){default:;}"), "switch (val){default:;}");
}

TEST(KeywordTest, Catch) {
    // catch 需要 try-catch 完整上下文才能被 tree-sitter 正确识别
    EXPECT_EQ(fmt("try{}catch(std::exception& e){}"),
              "try{}catch (std::exception &e){}");
}

// 非关键字后跟 '(' 不应受影响（如函数调用）
TEST(KeywordTest, NotFunctionCall) {
    EXPECT_EQ(fmt("func(arg)"), "func(arg)");
}

TEST(KeywordTest, Do) {
    // do-while 结构: do 已加入 is_keyword 列表作为防御性覆盖
    // C++ 合法语法中 do 后必须跟语句而非 '('，因此 keyword+'(' 规则不对 do 触发；
    // 此处通过格式化和 do-while 整体正确性验证
    EXPECT_EQ(fmt("do{}while(cond);"), "do{}while (cond);");
}

// ============================================================
// 直接验证 is_keyword() 静态方法（不含 tree-sitter 中间层）
// ============================================================

TEST(KeywordTest, IsKeywordIf)     { EXPECT_TRUE(TsFormatter::is_keyword("if")); }
TEST(KeywordTest, IsKeywordFor)    { EXPECT_TRUE(TsFormatter::is_keyword("for")); }
TEST(KeywordTest, IsKeywordWhile)  { EXPECT_TRUE(TsFormatter::is_keyword("while")); }
TEST(KeywordTest, IsKeywordSwitch) { EXPECT_TRUE(TsFormatter::is_keyword("switch")); }
TEST(KeywordTest, IsKeywordCatch)  { EXPECT_TRUE(TsFormatter::is_keyword("catch")); }
TEST(KeywordTest, IsKeywordDo)     { EXPECT_TRUE(TsFormatter::is_keyword("do")); }

TEST(KeywordTest, IsKeywordNonsense)     { EXPECT_FALSE(TsFormatter::is_keyword(nullptr)); }
TEST(KeywordTest, IsKeywordNonKeyword)   { EXPECT_FALSE(TsFormatter::is_keyword("func")); }
TEST(KeywordTest, IsKeywordTypeKeyword)  { EXPECT_FALSE(TsFormatter::is_keyword("int")); }
