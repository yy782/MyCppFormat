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
// 规范5: 括号内空格 — 去除括号内首尾空格
// ============================================================

TEST(BracketTest, If) {
    EXPECT_EQ(fmt("if ( x > 0 )"), "if (x > 0)");
}

TEST(BracketTest, For) {
    EXPECT_EQ(fmt("for ( int i = 0; i < 10; ++i )"),
              "for (int i = 0;i < 10;++i)");
}

TEST(BracketTest, While) {
    EXPECT_EQ(fmt("while ( x < 5 )"), "while (x < 5)");
}

// switch 需要完整语句（case/default + 块），不能孤立出现
TEST(BracketTest, Switch) {
    // 括号内空格被移除，分号规则同时去掉 ; 后空格
    EXPECT_EQ(fmt("switch ( x ) { case 1: break; }"),
              "switch (x) { case 1: break;}");
}
