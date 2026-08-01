#include "ts_formatter.hpp"

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
        if (std::strcmp(ptype, "pointer_declarator") == 0)         return true;
        if (std::strcmp(ptype, "reference_declarator") == 0)       return true;
        if (std::strcmp(ptype, "abstract_pointer_declarator") == 0) return true;
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
           std::strcmp(type, "catch") == 0;
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

    init_parser();

    // 解析源码
    TSTree *tree = ts_parser_parse_string(
        parser_, nullptr, source.data(),
        static_cast<uint32_t>(source.size()));

    // tree 为 nullptr 时回退为原样返回
    if (tree == nullptr) return source;

    TSNode root = ts_tree_root_node(tree);

    // 收集所有叶子 token
    std::vector<Token> tokens;
    collect_tokens(root, tokens);

    // 重构输出
    std::ostringstream out;
    uint32_t prev_end = 0;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto &tok = tokens[i];

        if (i > 0) {
            const auto &prev = tokens[i - 1];
            out << whitespace_between(prev, tok, source);
        } else {
            // 第一个 token 之前的内容（如有）
            out << std::string(source.begin(), source.begin() + tok.start_byte);
        }

        // 输出 token 文本
        out << std::string(source.begin() + tok.start_byte,
                           source.begin() + tok.end_byte);
        prev_end = tok.end_byte;
    }

    // 最后一个 token 之后的内容
    if (prev_end < source.size()) {
        out << std::string(source.begin() + prev_end, source.end());
    }

    ts_tree_delete(tree);
    return out.str();
}
