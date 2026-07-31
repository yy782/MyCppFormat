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
