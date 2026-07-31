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
// 测试场景13: { 后空格再接换行 — 不受影响（空格在新行前）
// { \n...} → 跨行，正则要求单行匹配，\n 阻断匹配
// ============================================================
TEST(ShortBodyTest, SpaceThenNewlineNotModified) {
    const char *input =
        "{ \n"
        "    return 0;\n"
        "}";
    EXPECT_EQ(fix_body(input), input);
}

// ============================================================
// 测试场景14: brace-initializer 不受影响
// auto v = { 1, 2 }; 内部无分号，不符合语句体特征，保持原样
// ============================================================
TEST(ShortBodyTest, BraceInitializerNotModified) {
    EXPECT_EQ(fix_body("auto v = { 1, 2 };"), "auto v = { 1, 2 };");
}

// ────────────────────────────────────────────────────────────
// Bug: 同一行嵌套大括号 — 外层语句体应被格式化
// 当前正则 [^\{\}\n] 遇到内层 { 即终止，
// 导致含嵌套大括号的外层 { } 完全不被匹配。
// 修复后外层首尾空格应被移除，内层 brace-initializer（无 ;）保持不变。
// ────────────────────────────────────────────────────────────

// ============================================================
// 测试场景15: 外层语句体包裹 brace-initializer
// 预期: { return {1, 2}; } → {return {1, 2};}
// Bug:  当前输出 { return {1, 2}; }（外层未被处理）
// ============================================================
TEST(ShortBodyTest, NestedBraceInitInBody) {
    EXPECT_EQ(fix_body("{ return {1, 2}; }"), "{return {1, 2};}");
}

// ============================================================
// 测试场景16: if 语句体包裹 brace-initializer
// 预期: if (x) { return {1, 2}; } → if (x) {return {1, 2};}
// Bug:  当前输出 if (x) { return {1, 2}; }
// ============================================================
TEST(ShortBodyTest, IfWithNestedBraceInit) {
    EXPECT_EQ(fix_body("if (x) { return {1, 2}; }"),
              "if (x) {return {1, 2};}");
}

// ============================================================
// 测试场景17: Lambda 表达式在语句体内
// 外层和内层 {} 均含分号，都是语句体，均应格式化。
// 预期: { return []{ return 0; }(); } → {return []{return 0;}();}
// Bug:  当前只改了内层 → { return []{return 0;}(); }
// ============================================================
TEST(ShortBodyTest, LambdaInsideBody) {
    EXPECT_EQ(fix_body("{ return []{ return 0; }(); }"),
              "{return []{return 0;}();}");
}

// ============================================================
// 测试场景18: 变量声明含 brace-initializer
// 预期: { auto v = {1, 2}; } → {auto v = {1, 2};}
// Bug:  当前输出 { auto v = {1, 2}; }
// ============================================================
TEST(ShortBodyTest, VarDeclWithBraceInit) {
    EXPECT_EQ(fix_body("{ auto v = {1, 2}; }"), "{auto v = {1, 2};}");
}

// ============================================================
// 测试场景19: 多层嵌套 brace-initializer
// 预期: { auto m = { {1}, {2} }; } → {auto m = { {1}, {2} };}
// Bug:  当前输出 { auto m = { {1}, {2} }; }
// ============================================================
TEST(ShortBodyTest, MultiNestedBraceInit) {
    EXPECT_EQ(fix_body("{ auto m = { {1}, {2} }; }"),
              "{auto m = { {1}, {2} };}");
}

// ============================================================
// 测试场景20: 构造函数式初始化在语句体内
// 预期: { std::vector<int> v{1, 2}; } → {std::vector<int> v{1, 2};}
// Bug:  当前输出 { std::vector<int> v{1, 2}; }
// ============================================================
TEST(ShortBodyTest, ConstructorInitInBody) {
    EXPECT_EQ(fix_body("{ std::vector<int> v{1, 2}; }"),
              "{std::vector<int> v{1, 2};}");
}

// ============================================================
// 测试场景21: Lambda 在 if 语句体内
// if 体与 Lambda 体内层 {} 均含分号，均应格式化。
// 预期: if (x) { auto f = []{ do_something(); }; } → if (x) {auto f = []{do_something();};}
// Bug:  当前只改了内层
// ============================================================
TEST(ShortBodyTest, LambdaInIfBody) {
    EXPECT_EQ(fix_body("if (x) { auto f = []{ do_something(); }; }"),
              "if (x) {auto f = []{do_something();};}");
}
