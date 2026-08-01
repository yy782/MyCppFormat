#include "ts_formatter.hpp"

#include <cctype>
#include <cstring>
#include <sstream>
#include <stdexcept>

// tree-sitter-cpp 语言定义（由 tree-sitter-cpp 库提供）
extern "C" {
TSLanguage *tree_sitter_cpp();
}

// ============================================================
// 构造 / 析构
// ============================================================

TsFormatter::TsFormatter()  = default;
TsFormatter::~TsFormatter() { destroy_parser(); }

// ── 移动语义（接管 parser_ 所有权） ──
TsFormatter::TsFormatter(TsFormatter&& other) noexcept : parser_(other.parser_) {
    other.parser_ = nullptr;
}

TsFormatter& TsFormatter::operator=(TsFormatter&& other) noexcept {
    if (this != &other) {
        destroy_parser();
        parser_ = other.parser_;
        other.parser_ = nullptr;
    }
    return *this;
}

// ── parser 生命周期管理 ──

void TsFormatter::init_parser() {
    if (parser_ != nullptr) return;
    parser_ = ts_parser_new();
    if (parser_ == nullptr) {
        throw std::runtime_error("TsFormatter: ts_parser_new() 返回 nullptr");
    }
    if (!ts_parser_set_language(parser_, tree_sitter_cpp())) {
        destroy_parser();
        throw std::runtime_error("TsFormatter: ts_parser_set_language() 失败");
    }
}

void TsFormatter::destroy_parser() {
    if (parser_ != nullptr) {
        ts_parser_delete(parser_);
        parser_ = nullptr;
    }
}

// ============================================================
// Token 收集：递归遍历 CST，收集所有叶子节点
// ============================================================

void TsFormatter::collect_tokens(TSNode node, std::vector<Token> &tokens) {
    uint32_t child_count = ts_node_child_count(node);
    if (child_count == 0) {
        // 叶子节点：记录字节范围和 node 引用
        tokens.push_back({
            ts_node_start_byte(node),
            ts_node_end_byte(node),
            node,
        });
        return;
    }
    // 非叶子节点：递归遍历所有子节点
    for (uint32_t i = 0; i < child_count; ++i) {
        collect_tokens(ts_node_child(node, i), tokens);
    }
}

// ============================================================
// 辅助：空白符处理
// ============================================================

std::string TsFormatter::keep_newlines_only(const std::string &gap) {
    std::string result;
    for (char c : gap) {
        if (c == '\n' || c == '\r') result += c;
    }
    return result;
}

/// 移除同行水平空白；若原 gap 跨行则原样返回以保留缩进
static std::string strip_ws_same_line(const std::string &gap) {
    if (gap.find('\n') != std::string::npos) return gap;  // 跨行 → 保留缩进
    return "";  // 同行 → 移除水平空白
}

// ============================================================
// 辅助：节点类型检测
// ============================================================

/// 检查声明符链是否最终落于 identifier（变量名）
/// 如 int *p 中，* 后面的链是 identifier(p) → 是变量声明
/// 如 V* operator->() 中，* 后面的链是 function_declarator → 是返回类型
static bool declarator_resolves_to_identifier(TSNode decl) {
    while (!ts_node_is_null(decl)) {
        const char *dtype = ts_node_type(decl);
        if (dtype == nullptr) break;
        if (std::strcmp(dtype, "identifier") == 0 ||
            std::strcmp(dtype, "field_identifier") == 0) return true;
        // 多级指针/引用：继续向下走进内层声明符
        if (std::strcmp(dtype, "pointer_declarator") == 0 ||
            std::strcmp(dtype, "reference_declarator") == 0) {
            // 找到内层 * / & 后面的声明符
            uint32_t dc = ts_node_child_count(decl);
            bool found_next = false;
            TSNode next;
            for (uint32_t j = 0; j + 1 < dc; j++) {
                const char *ct = ts_node_type(ts_node_child(decl, j));
                if (ct && (std::strcmp(ct, "*") == 0 ||
                           std::strcmp(ct, "&") == 0 ||
                           std::strcmp(ct, "&&") == 0)) {
                    next = ts_node_child(decl, j + 1);
                    found_next = true;
                    break;
                }
            }
            if (!found_next) break;
            decl = next;
            continue;
        }
        // function_declarator / operator_name / 其他 → 不是变量声明
        break;
    }
    return false;
}

bool TsFormatter::in_pointer_or_ref_decl(TSNode node) {
    const char *type = ts_node_type(node);
    if (type == nullptr) return false;
    if (std::strcmp(type, "*") != 0 && std::strcmp(type, "&") != 0 &&
        std::strcmp(type, "&&") != 0) return false;

    // 向上查找指针/引用相关的声明符节点
    TSNode parent = ts_node_parent(node);
    while (!ts_node_is_null(parent)) {
        const char *ptype = ts_node_type(parent);
        if (ptype == nullptr) break;

        if (std::strcmp(ptype, "pointer_declarator") == 0 ||
            std::strcmp(ptype, "reference_declarator") == 0 ||
            std::strcmp(ptype, "abstract_pointer_declarator") == 0) {
            // 在 parent 中找到 * / & 之后紧跟的声明符
            uint32_t child_count = ts_node_child_count(parent);
            TSNode decl = parent; // fallback: 会被后面的循环覆盖
            bool found_decl = false;
            for (uint32_t i = 0; i + 1 < child_count; i++) {
                TSNode child = ts_node_child(parent, i);
                if (ts_node_start_byte(child) == ts_node_start_byte(node)) {
                    decl = ts_node_child(parent, i + 1);
                    found_decl = true;
                    break;
                }
            }
            // 检查声明符链是否最终指向变量名
            if (!found_decl) return false;
            return declarator_resolves_to_identifier(decl);
        }
        parent = ts_node_parent(parent);
    }
    return false;
}

bool TsFormatter::is_keyword(const char *type) {
    if (type == nullptr) return false;
    return std::strcmp(type, "if")    == 0 ||
           std::strcmp(type, "for")   == 0 ||
           std::strcmp(type, "while") == 0 ||
           std::strcmp(type, "switch") == 0 ||
           std::strcmp(type, "catch") == 0 ||
           std::strcmp(type, "do")    == 0;
}

/// 检查 { 或 } 是否属于函数定义的 compound_statement 定界符
/// （排除 if/for/while/switch/try-catch 的复合语句块，
///   以及 braced initializer 如 return { 1 };）
/// 必须同时满足：直接父节点是 compound_statement，且该
/// compound_statement 的父节点是 function_definition
static bool in_function_body(TSNode node) {
    const char *type = ts_node_type(node);
    if (type == nullptr) return false;
    if (std::strcmp(type, "{") != 0 && std::strcmp(type, "}") != 0) return false;

    TSNode parent = ts_node_parent(node);
    if (ts_node_is_null(parent)) return false;

    const char *ptype = ts_node_type(parent);
    if (ptype == nullptr || std::strcmp(ptype, "compound_statement") != 0) {
        return false;
    }

    TSNode gp = ts_node_parent(parent);
    if (ts_node_is_null(gp)) return false;

    const char *gptype = ts_node_type(gp);
    return gptype != nullptr &&
           std::strcmp(gptype, "function_definition") == 0;
}

bool TsFormatter::in_error_subtree(TSNode node) {
    // 仅检查节点本身或祖先是否为 ERROR 类型（而非 ts_node_has_error
    // 因为它会对含 ERROR 后代的正常节点也返回 true）
    TSNode cur = node;
    while (!ts_node_is_null(cur)) {
        if (std::strcmp(ts_node_type(cur), "ERROR") == 0) return true;
        cur = ts_node_parent(cur);
    }
    return false;
}

// ============================================================
// 宏体保护：扫描 #define 的 body 字节范围，避免 tree-sitter
// 将宏体中的 token 当作普通 C++ token 解析导致空白符错误
// ============================================================

/// 扫描源码行，找出所有 #define 宏体的字节范围 (body_start, body_end)
/// body 指宏名（及函数式宏的形参列表）之后直到指令末尾的内容，
/// 支持反斜杠续行。空 body 的宏会被跳过。
static std::vector<std::pair<uint32_t, uint32_t>>
find_macro_bodies(const std::string &source) {
    std::vector<std::pair<uint32_t, uint32_t>> bodies;
    const char *s = source.data();
    uint32_t len = static_cast<uint32_t>(source.size());
    uint32_t i = 0;

    while (i < len) {
        // 跳过行首空白
        while (i < len && (s[i] == ' ' || s[i] == '\t')) ++i;

        // 检查 #define 模式: # + 可选空白 + define + 空白
        if (i >= len || s[i] != '#') goto skip_line;

        {
            uint32_t j = i + 1;
            while (j < len && (s[j] == ' ' || s[j] == '\t')) ++j;
            if (j + 6 > len || std::strncmp(s + j, "define", 6) != 0)
                goto skip_line;
            j += 6;
            // "define" 后必须是分隔符或行尾
            if (j < len && s[j] != ' ' && s[j] != '\t' && s[j] != '\n' && s[j] != '\r')
                goto skip_line;

            // 跳过空白至宏名
            while (j < len && (s[j] == ' ' || s[j] == '\t')) ++j;
            if (j >= len) goto skip_line;

            // 跳过宏名
            while (j < len && (std::isalnum(static_cast<unsigned char>(s[j])) || s[j] == '_')) ++j;

            // 函数式宏: 跳过 (...) 形参列表
            if (j < len && s[j] == '(') {
                int depth = 1;
                ++j;  // 跳过 '('
                while (j < len && depth > 0) {
                    if (s[j] == '\\') { j += 2; continue; }  // 跳过转义
                    if (s[j] == '(') ++depth;
                    else if (s[j] == ')') --depth;
                    ++j;
                }
            }

            uint32_t body_start = j;

            // 收集所有续行（以 \ 结尾的行）。用 line_begin 跟踪每行起点，
            // 避免反向扫描越过 \n 进入前行误判续行符（如纯空白行）。
            uint32_t line_begin = body_start;
            while (j < len) {
                // 找到当前行的行尾
                while (j < len && s[j] != '\n' && s[j] != '\r') ++j;

                // 检查行尾前（跳过结尾空白）是否为 '\'，限定在当前行范围内
                uint32_t k = j;
                while (k > line_begin && (s[k - 1] == ' ' || s[k - 1] == '\t')) --k;
                bool cont = (k > line_begin && s[k - 1] == '\\');

                // 跳过换行符
                if (j < len && s[j] == '\r') ++j;
                if (j < len && s[j] == '\n') ++j;

                if (!cont) break;

                // 下一行续行的起点
                line_begin = j;
            }

            uint32_t body_end = j;

            // 排除空 body（仅含空白和 \ 续行符），避免将换行符标记为 body 范围
            if (body_end > body_start) {
                bool has_content = false;
                for (uint32_t p = body_start; p < body_end; ++p) {
                    char c = s[p];
                    if (c != ' ' && c != '\t' && c != '\n' && c != '\r' && c != '\\') {
                        has_content = true;
                        break;
                    }
                }
                if (has_content) {
                    bodies.emplace_back(body_start, body_end);
                }
            }
            i = j;  // 已跳过整个宏定义
            continue;
        }

    skip_line:
        while (i < len && s[i] != '\n' && s[i] != '\r') ++i;
        if (i < len && s[i] == '\r') ++i;
        if (i < len && s[i] == '\n') ++i;
    }

    return bodies;
}

/// 判断 [gap_start, gap_end) 是否与任一宏体范围有交集
static bool overlaps_macro_body(uint32_t gap_start, uint32_t gap_end,
    const std::vector<std::pair<uint32_t, uint32_t>> &bodies) {
    for (const auto &b : bodies) {
        if (gap_start < b.second && gap_end > b.first) return true;
    }
    return false;
}

// ============================================================
// 空白符决策（核心规则引擎）
// ============================================================

std::string TsFormatter::whitespace_between(
    const Token &prev, const Token &cur, const std::string &source) {

    const std::string original_gap(
        source.begin() + prev.end_byte,
        source.begin() + cur.start_byte);

    const char *prev_type = ts_node_type(prev.node);
    const char *cur_type  = ts_node_type(cur.node);

    // 如果任意 token 在 ERROR 子树中，保持原样
    if (in_error_subtree(prev.node) || in_error_subtree(cur.node)) {
        return original_gap;
    }

    // ── 规范2: 逗号分隔 — 逗号后不加空格 ──
    if (prev_type != nullptr && std::strcmp(prev_type, ",") == 0) {
        return strip_ws_same_line(original_gap);
    }

    // ── 规范3: 分号分隔 — 分号后不加水平空格 ──
    if (prev_type != nullptr && std::strcmp(prev_type, ";") == 0) {
        return strip_ws_same_line(original_gap);
    }

    // ── 规范5: 括号内空格 — 去除 '(' 后和 ')' 前的水平空格 ──
    if (prev_type != nullptr && std::strcmp(prev_type, "(") == 0) {
        return strip_ws_same_line(original_gap);
    }
    if (cur_type != nullptr && std::strcmp(cur_type, ")") == 0) {
        return strip_ws_same_line(original_gap);
    }

    // ── 规范4: 短函数体 — 单行函数体大括号内去除首尾空格 ──
    // 仅对 function_definition 内的 compound_statement 生效
    if (prev_type != nullptr && std::strcmp(prev_type, "{") == 0 &&
        in_function_body(prev.node)) {
        return strip_ws_same_line(original_gap);
    }
    if (cur_type != nullptr && std::strcmp(cur_type, "}") == 0 &&
        in_function_body(cur.node)) {
        return strip_ws_same_line(original_gap);
    }

    // ── 规范1: 指针/引用对齐 ──
    // 后置空格：前一个 token 是指针/引用 → 移除后置空格（处理 ** 等连续情况）
    if (in_pointer_or_ref_decl(prev.node)) {
        return strip_ws_same_line(original_gap);
    }
    // 前置空格：当前 token 是指针/引用 → 同行加一个空格，跨行保留换行和缩进
    if (in_pointer_or_ref_decl(cur.node)) {
        if (original_gap.find('\n') != std::string::npos) {
            return original_gap;  // 跨行 → 不折叠
        }
        return " ";  // 同行 → 单个空格
    }

    // ── 规范6: 关键字空格 — 关键字与 '(' 间保留一个空格 ──
    if (is_keyword(prev_type) && cur_type != nullptr &&
        std::strcmp(cur_type, "(") == 0) {
        return " ";
    }

    // 默认：保持原样
    return original_gap;
}

// ============================================================
// 公开接口：format()
// ============================================================

std::string TsFormatter::format(const std::string &source) {
    if (source.empty()) return source;

    // ─ 阶段1: 扫描 #define 宏体范围 ─
    auto macro_bodies = find_macro_bodies(source);

    // ─ 阶段2: 构造掩码源 — 宏体替换为等长空格，保留 \ 和换行 ─
    std::string masked = source;
    for (const auto &b : macro_bodies) {
        for (uint32_t p = b.first; p < b.second; ++p) {
            char c = masked[p];
            if (c == '\\' || c == '\n' || c == '\r') continue;
            masked[p] = ' ';
        }
    }

    // ─ 阶段2b: 屏蔽所有 # 预处理指令行（含 #include, #if, #define 等）──
    // tree-sitter-cpp 无法解析预处理指令，需将其整行替换为空格，
    // 避免产生 ERROR 子树导致格式化规则被跳过。
    {
        const char *s = masked.data();
        uint32_t len = static_cast<uint32_t>(masked.size());
        uint32_t i = 0;
        while (i < len) {
            while (i < len && (s[i] == ' ' || s[i] == '\t')) ++i;
            if (i >= len) break;
            if (s[i] != '#') {
                while (i < len && s[i] != '\n' && s[i] != '\r') ++i;
                if (i < len && s[i] == '\r') ++i;
                if (i < len && s[i] == '\n') ++i;
                continue;
            }
            uint32_t line_end = i;
            while (line_end < len && s[line_end] != '\n' && s[line_end] != '\r') ++line_end;
            for (uint32_t p = i; p < line_end; ++p) masked[p] = ' ';
            i = line_end;
            if (i < len && s[i] == '\r') ++i;
            if (i < len && s[i] == '\n') ++i;
        }
    }

    init_parser();

    // ─ 阶段3: 对掩码源使用 tree-sitter 解析 ─
    TSTree *tree = ts_parser_parse_string(
        parser_, nullptr, masked.data(),
        static_cast<uint32_t>(masked.size()));

    if (tree == nullptr) return source;

    TSNode root = ts_tree_root_node(tree);

    // 收集所有叶子 token（基于掩码源，但字节偏移与原始源一致）
    std::vector<Token> tokens;
    collect_tokens(root, tokens);

    // ─ 阶段4: 重构输出 — token 文本和空白均从原始 source 读取；
    //          若 gap 与宏体范围有交集则保持原样，否则应用格式化规则 ─
    std::ostringstream out;
    uint32_t prev_end = 0;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto &tok = tokens[i];

        if (i > 0) {
            const auto &prev = tokens[i - 1];
            uint32_t gap_start = prev.end_byte;
            uint32_t gap_end   = tok.start_byte;

            if (overlaps_macro_body(gap_start, gap_end, macro_bodies)) {
                // 宏体范围内 — 保持原始空白不动
                out << std::string(source.begin() + gap_start,
                                   source.begin() + gap_end);
            } else {
                out << whitespace_between(prev, tok, source);
            }
        } else {
            // 首个 token 之前的内容
            out << std::string(source.begin(), source.begin() + tok.start_byte);
        }

        // 输出 token 文本（从原始源读取，掩码区不会有 token）
        out << std::string(source.begin() + tok.start_byte,
                           source.begin() + tok.end_byte);
        prev_end = tok.end_byte;
    }

    // 尾部残余内容
    if (prev_end < source.size()) {
        out << std::string(source.begin() + prev_end, source.end());
    }

    ts_tree_delete(tree);
    return out.str();
}
