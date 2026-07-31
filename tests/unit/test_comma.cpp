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
// 规范2: 逗号分隔 — 逗号后不加空格
// ============================================================

TEST(CommaTest, FuncArgs) {
    EXPECT_EQ(fmt("func(a, b, c)"), "func(a,b,c)");
}

TEST(CommaTest, VarDecl) {
    EXPECT_EQ(fmt("int a, b, c;"), "int a,b,c;");
}

TEST(CommaTest, MultipleSpaces) {
    EXPECT_EQ(fmt("func(a,   b,     c)"), "func(a,b,c)");
}
