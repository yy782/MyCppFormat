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
// 规范3: 分号分隔 — 分号后不加空格
// ============================================================

TEST(SemicolonTest, ForLoop) {
    EXPECT_EQ(fmt("for (int i = 0; i < 10; ++i)"),
              "for (int i = 0;i < 10;++i)");
}

TEST(SemicolonTest, MultipleSpaces) {
    EXPECT_EQ(fmt("int a;   int b;"), "int a;int b;");
}

// ============================================================
// 附带分号规则的恒等测试
// ============================================================

// 分号规则触发：`; }` → `;}`；同时短函数体规则触发：`{ ` → `{`
TEST(SemicolonTest, IdentitySimpleFunc) {
    EXPECT_EQ(fmt("int f() { return 0; }"), "int f() {return 0;}");
}

// 分号规则触发：`a; //` → `a;//`
TEST(SemicolonTest, IdentityLineComment) {
    EXPECT_EQ(fmt("int a; // int* b, c; if()"),
              "int a;// int* b, c; if()");
}
