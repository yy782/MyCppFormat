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

// ============================================================
// 测试场景11: 多行体，{ 后紧跟块注释 — 不受影响
// protect() 将 /*c*/ 替换为占位符，但 [^\n]+? 在 \n 前截止，
// 无法跨行匹配 }，整个正则失败，文本保持不变
// ============================================================
TEST(ShortBodyTest, MultiLineWithBlockComment) {
    const char *input =
        "{ /*c*/\n"
        "    x;\n"
        "}";
    EXPECT_EQ(fix_body(input), input);
}

// ============================================================
// 测试场景12: 多行体，{ 后紧跟行注释 — 不受影响
// 原理同测试11，行注释的占位符后跟 \n，阻断单行匹配
// ============================================================
TEST(ShortBodyTest, MultiLineWithLineComment) {
    const char *input =
        "if (x) { //c\n"
        "    y;\n"
        "}";
    EXPECT_EQ(fix_body(input), input);
}

// ============================================================
// 测试场景13: brace-initializer — 单行内空格被移除
// auto v = { 1, 2 }; → auto v = {1, 2};
// 虽然是初始化列表而非函数体，但单行 {} 内空格清理保持一致
// ============================================================
TEST(ShortBodyTest, BraceInitializer) {
    EXPECT_EQ(fix_body("auto v = { 1, 2 };"), "auto v = {1, 2};");
}

// ============================================================
// 测试场景14: { 后空格再接换行 — 不受影响（空格在新行前）
// { \n...} → 空格是 { 后唯一内容，[^\n]+? 无字符可匹配，
// 随后 \n 阻断了对 } 的匹配
// ============================================================
TEST(ShortBodyTest, SpaceThenNewlineNotModified) {
    const char *input =
        "{ \n"
        "    return 0;\n"
        "}";
    EXPECT_EQ(fix_body(input), input);
}
