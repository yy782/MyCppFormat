#include "formatter.hpp"

#include <regex>
#include <sstream>

// ============================================================
// 敏感区域保护
// ============================================================

// 占位符前缀 — 选择 C++ 中几乎不可能出现的字符串
static const char *PH_PREFIX = "\x01PH";

std::string Formatter::protect(const std::string &source) {
    placeholders_.clear();
    std::string result = source;

    const std::vector<std::pair<std::regex, std::string>> patterns = {
        {std::regex(R"("(?:[^"\\]|\\.)*")"), "string"},
        {std::regex(R"('(?:[^'\\]|\\.)*')"), "char"},
        {std::regex(R"(/\*[\s\S]*?\*/)"), "block_comment"},
        {std::regex(R"(//[^\n]*)"), "line_comment"},
        {std::regex(R"(^\s*#[^\n]*)", std::regex::multiline), "preprocessor"},
    };

    int id = 0;
    for (const auto &[re, label] : patterns) {
        std::string replaced;
        std::smatch m;
        auto it = result.cbegin();
        while (std::regex_search(it, result.cend(), m, re)) {
            replaced.append(it, m[0].first);
            // 使用 \x01 开头避免与源码混淆，格式: \x01PH+id+\x01
            std::string ph;
            ph += static_cast<char>(1);
            ph += "PH";
            ph += std::to_string(id);
            ph += static_cast<char>(1);
            replaced += ph;
            placeholders_.push_back(m[0].str());
            ++id;
            it = m[0].second;
        }
        replaced.append(it, result.cend());
        result = std::move(replaced);
    }
    return result;
}

std::string Formatter::restore(const std::string &source) {
    std::string result = source;
    for (int i = static_cast<int>(placeholders_.size()) - 1; i >= 0; --i) {
        std::string ph;
        ph += static_cast<char>(1);
        ph += "PH";
        ph += std::to_string(i);
        ph += static_cast<char>(1);

        size_t pos = result.find(ph);
        if (pos != std::string::npos) {
            result.replace(pos, ph.size(), placeholders_[i]);
        }
    }
    return result;
}

// ============================================================
// 格式化规则（内部，操作已保护文本）
// ============================================================

std::string Formatter::applyPointerAlignment(const std::string &source) {
    std::string text = source;

    // 规则1a: 复合类型，如 const char* msg → const char *msg
    {
        const char *pat =
            "(const|volatile|static|unsigned|signed|long|short)"
            "\\s+(\\w+)([*&]+)\\s+([a-zA-Z_]\\w*)";
        std::regex re(pat);
        text = std::regex_replace(text, re, "$1 $2 $3$4");
    }

    // 规则1b: type* name → type *name（基础类型后紧跟 *& 再接标识符）
    {
        const char *pat =
            "\\b(int|char|double|float|void|bool|auto|size_t|wchar_t)"
            "([*&]+)\\s+([a-zA-Z_]\\w*)";
        std::regex re(pat);
        text = std::regex_replace(text, re, "$1 $2$3");
    }

    // 规则1c: type * name → type *name（*& 和变量名之间有空格时移除）
    {
        const char *pat =
            "\\b(int|char|double|float|void|bool|auto|size_t|wchar_t"
            "|const|volatile|static|unsigned|signed|long|short)"
            "\\s+([*&]+)\\s+([a-zA-Z_]\\w*)";
        std::regex re(pat);
        text = std::regex_replace(text, re, "$1 $2$3");
    }

    // 规则1d: , * name 或 ( * name → ,*name 或 (*name
    {
        const char *pat = "([,()])\\s*([*&]+)\\s+([a-zA-Z_]\\w*)";
        std::regex re(pat);
        text = std::regex_replace(text, re, "$1$2$3");
    }

    return text;
}

// ============================================================
// 公开接口
// ============================================================

std::string Formatter::fixPointerAlignment(const std::string &source) {
    std::string text = protect(source);
    text = applyPointerAlignment(text);
    return restore(text);
}

std::string Formatter::format(const std::string &source) {
    std::string text = protect(source);
    // 依次应用所有格式化规则
    text = applyPointerAlignment(text);
    return restore(text);
}
