#include "formatter.hpp"
#include <gtest/gtest.h>

// 辅助函数：仅调用短函数体规则
static std::string fix_body(const std::string &src) {
    return Formatter().fixShortBody(src);
}

// ============================================================
// 测试场景1: 单行函数体首尾空格 — 均移除
// 预期: { return true; } → {return true;}
// ============================================================
TEST(ShortBodyTest, SingleLineBodySpaces) {
    EXPECT_EQ(fix_body("{ return true; }"), "{return true;}");
}

// ============================================================
// 测试场景2: 仅左大括号后有空格
// 预期: { return true;} → {return true;}
// ============================================================
TEST(ShortBodyTest, LeadingSpaceOnly) {
    EXPECT_EQ(fix_body("{ return true;}"), "{return true;}");
}

// ============================================================
// 测试场景3: 仅右大括号前有空格
// 预期: {return true; } → {return true;}
// ============================================================
TEST(ShortBodyTest, TrailingSpaceOnly) {
    EXPECT_EQ(fix_body("{return true; }"), "{return true;}");
}

// ============================================================
// 测试场景4: 空大括号
// 预期: {  } → {}
// ============================================================
TEST(ShortBodyTest, EmptyBraces) {
    EXPECT_EQ(fix_body("{  }"), "{}");
}

// ============================================================
// 测试场景5: 制表符
// 预期: {\treturn true;\t} → {return true;}
// ============================================================
TEST(ShortBodyTest, TabInsideBraces) {
    EXPECT_EQ(fix_body("{\treturn true;\t}"), "{return true;}");
}

// ============================================================
// 测试场景6: 多行大括号不受影响
// 预期: 每个内部行保持原有缩进
// ============================================================
TEST(ShortBodyTest, MultiLineNotModified) {
    const char *input =
        "{\n"
        "    return true;\n"
        "}";
    EXPECT_EQ(fix_body(input), input);
}

// ============================================================
// 测试场景7: 无空格保持
// 预期: {return true;} → {return true;}
// ============================================================
TEST(ShortBodyTest, AlreadyCorrect) {
    EXPECT_EQ(fix_body("{return true;}"), "{return true;}");
}

// ============================================================
// 测试场景8: 字符串内大括号不受影响
// 预期: const char *s = "{ content }"; 保持不变
// ============================================================
TEST(ShortBodyTest, StringLiteralNotModified) {
    EXPECT_EQ(fix_body("const char *s = \"{ content }\";"),
              "const char *s = \"{ content }\";");
}

// ============================================================
// 测试场景9: if 控制流 body
// 预期: if (x) { return x; } → if (x) {return x;}
// ============================================================
TEST(ShortBodyTest, IfBody) {
    EXPECT_EQ(fix_body("if (x) { return x; }"),
              "if (x) {return x;}");
}

// ============================================================
// 测试场景10: 多组大括号混合
// 预期: 各组独立处理
// ============================================================
TEST(ShortBodyTest, MultipleBracePairs) {
    EXPECT_EQ(fix_body("int f() { return 0; } int g() { return 1; }"),
              "int f() {return 0;} int g() {return 1;}");
}
