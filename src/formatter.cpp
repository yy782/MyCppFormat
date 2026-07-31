#include "formatter.hpp"

#include <algorithm>
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

    int id = 0;

    // ── 原始字符串字面量 ──────────────────────────────────────
    // C++ raw strings: R"(... )", R"delim(... )delim"
    // 支持前缀: u8, u, U, L
    // std::regex 对带反向引用的模式支持有限，故手动扫描。
    {
        static const std::regex raw_re(R"((?:u8|u|U|L)?R")");
        std::string replaced;
        std::smatch m;
        auto it = result.cbegin();
        while (std::regex_search(it, result.cend(), m, raw_re)) {
            auto match_start = m[0].first;
            auto pos         = m[0].second;   // 指向 R" 之后

            // 解析分隔符: R" 之后到 ( 之间的字符
            std::string delim;
            while (pos != result.cend() && *pos != '(') {
                delim += *pos;
                ++pos;
            }
            if (pos == result.cend()) {
                // 畸形，跳过此匹配继续
                replaced.append(it, m[0].second);
                it = m[0].second;
                continue;
            }
            ++pos;  // 跳过 '('

            // 闭合序列: ) + delim + "
            std::string closer = ")" + delim + "\"";
            auto close_pos = std::search(pos, result.cend(),
                                          closer.begin(), closer.end());
            if (close_pos == result.cend()) {
                // 畸形，保留匹配到的原始文本（含前缀）
                replaced.append(it, m[0].first);
                replaced += std::string(m[0].first, m[0].second) + delim + "(";
                it = pos;
                continue;
            }
            auto match_end = close_pos + closer.size();

            replaced.append(it, match_start);
            std::string ph = std::string("\x01PH", 3) + std::to_string(id) + '\x01';
            replaced += ph;
            placeholders_.push_back(std::string(match_start, match_end));
            ++id;
            it = match_end;
        }
        replaced.append(it, result.cend());
        result = std::move(replaced);
    }

    // 基础模式：字符串、字符、注释（预处理指令下面单独处理）
    const std::vector<std::pair<std::regex, std::string>> patterns = {
        {std::regex(R"("(?:[^"\\]|\\.)*")"), "string"},
        {std::regex(R"('(?:[^'\\]|\\.)*')"), "char"},
        {std::regex(R"(/\*[\s\S]*?\*/)"), "block_comment"},
        {std::regex(R"(//[^\n]*)"), "line_comment"},
    };

    for (const auto &[re, label] : patterns) {
        std::string replaced;
        std::smatch m;
        auto it = result.cbegin();
        while (std::regex_search(it, result.cend(), m, re)) {
            replaced.append(it, m[0].first);
            std::string ph = std::string("\x01PH", 3) + std::to_string(id) + '\x01';
            replaced += ph;
            placeholders_.push_back(m[0].str());
            ++id;
            it = m[0].second;
        }
        replaced.append(it, result.cend());
        result = std::move(replaced);
    }

    // 预处理器指令：手动处理反斜杠续行。
    // std::regex 引擎不支持此类回溯匹配，故用迭代方式扫描续行。
    {
        std::regex re(R"(^\s*#[^\n]*)", std::regex::multiline);
        std::string replaced;
        std::smatch m;
        auto it = result.cbegin();
        while (std::regex_search(it, result.cend(), m, re)) {
            auto match_start = m[0].first;
            auto match_end   = m[0].second;

            // 向前扫描反斜杠续行：行末有 \ 则吞下换行和续行内容
            while (match_end != result.cend()) {
                auto last = *(match_end - 1);
                if (last != '\\') break;
                if (match_end == result.cend()) break;
                if (*match_end == '\n') {
                    ++match_end;               // 吞掉换行
                    while (match_end != result.cend() && *match_end != '\n')
                        ++match_end;           // 吞掉续行内容
                } else {
                    break;
                }
            }

            replaced.append(it, match_start);
            std::string ph = std::string("\x01PH", 3) + std::to_string(id) + '\x01';
            replaced += ph;
            placeholders_.push_back(std::string(match_start, match_end));
            ++id;
            it = match_end;
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

    // 规则1e: >> * name → >> *name（先处理嵌套模板，避免被 1f 误消费）
    {
        const char *pat = "(>>)\\s*([*&]+)\\s*([a-zA-Z_]\\w*)";
        std::regex re(pat);
        text = std::regex_replace(text, re, "$1 $2$3");
    }

    // 规则1f: > * name → > *name（单层模板类型后的指针/引用对齐）
    {
        const char *pat = "(>)\\s*([*&]+)\\s*([a-zA-Z_]\\w*)";
        std::regex re(pat);
        text = std::regex_replace(text, re, "$1 $2$3");
    }

    return text;
}

// ============================================================
// 规范2: 逗号分隔 — 逗号后面不加空格
// ============================================================

std::string Formatter::applyCommaSpacing(const std::string &source) {
    // 匹配逗号后紧跟一个或多个空白字符，替换为逗号
    static const std::regex re(",\\s+");
    return std::regex_replace(source, re, ",");
}

// ============================================================
// 规范3: 分号分隔 — 分号后面不加空格
// ============================================================

std::string Formatter::applySemicolonSpacing(const std::string &source) {
    // 匹配分号后紧跟一个或多个空格/制表符（不含换行），替换为分号
    static const std::regex re(";[ \\t]+");
    return std::regex_replace(source, re, ";");
}

// ============================================================
// 规范5: 括号内空格 — 去除括号内首尾空格
// ============================================================

std::string Formatter::applyBracketSpace(const std::string &source) {
    std::string text = source;

    // 去除 '(' 后的空白
    {
        static const std::regex re("\\([ \\t]+");
        text = std::regex_replace(text, re, "(");
    }

    // 去除 ')' 前的空白
    {
        static const std::regex re("[ \\t]+\\)");
        text = std::regex_replace(text, re, ")");
    }

    return text;
}

// ============================================================
// 规范6: 关键字空格 — 关键字与左括号间保留一个空格
// ============================================================

std::string Formatter::applyKeywordSpace(const std::string &source) {
    // 匹配关键字后可选空白紧接 '('，替换为 "关键字 ("
    // \b 确保是独立单词，不会误匹配 my_if 等
    static const std::regex re(
        R"(\b(if|for|while|switch|catch)\s*\()"
    );
    return std::regex_replace(source, re, "$1 (");
}

// ============================================================
// 公开接口
// ============================================================

std::string Formatter::fixPointerAlignment(const std::string &source) {
    std::string text = protect(source);
    text = applyPointerAlignment(text);
    return restore(text);
}

std::string Formatter::fixCommaSpacing(const std::string &source) {
    std::string text = protect(source);
    text = applyCommaSpacing(text);
    return restore(text);
}

std::string Formatter::fixSemicolonSpacing(const std::string &source) {
    std::string text = protect(source);
    text = applySemicolonSpacing(text);
    return restore(text);
}

std::string Formatter::fixBracketSpace(const std::string &source) {
    std::string text = protect(source);
    text = applyBracketSpace(text);
    return restore(text);
}

std::string Formatter::fixKeywordSpace(const std::string &source) {
    std::string text = protect(source);
    text = applyKeywordSpace(text);
    return restore(text);
}

std::string Formatter::format(const std::string &source) {
    std::string text = protect(source);
    // 依次应用所有格式化规则
    text = applyPointerAlignment(text);
    text = applyCommaSpacing(text);
    text = applySemicolonSpacing(text);
    text = applyBracketSpace(text);
    text = applyKeywordSpace(text);
    return restore(text);
}
