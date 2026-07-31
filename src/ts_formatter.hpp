#ifndef MYCPPFORMAT_TS_FORMATTER_HPP
#define MYCPPFORMAT_TS_FORMATTER_HPP

#include <string>
#include <vector>

extern "C" {
#include <tree_sitter/api.h>
}

/// 基于 Tree-sitter CST 遍历的 C++ 格式化器
class TsFormatter {
public:
    TsFormatter();
    ~TsFormatter();

    // 禁止拷贝（持有原始指针 parser_，拷贝会导致 double-free）
    TsFormatter(const TsFormatter&) = delete;
    TsFormatter& operator=(const TsFormatter&) = delete;

    // 支持移动（移动后源对象 parser_ 置为 nullptr）
    TsFormatter(TsFormatter&& other) noexcept;
    TsFormatter& operator=(TsFormatter&& other) noexcept;

    /// 对 C++ 源码应用所有格式规则，返回格式化后的文本
    std::string format(const std::string &source);

private:
    // ── 解析 ──
    void init_parser();
    void destroy_parser();

    // ── Token 信息 ──
    struct Token {
        uint32_t start_byte;
        uint32_t end_byte;
        TSNode   node;
    };

    /// 递归收集所有叶子 token（按源码顺序）
    void collect_tokens(TSNode node, std::vector<Token> &tokens);

    // ── 空白符决策 ──
    /// 仅保留换行符，去除水平空白
    static std::string keep_newlines_only(const std::string &gap);

    /// 检查 token 是否位于 pointer_declarator 或 reference_declarator 内
    static bool in_pointer_or_ref_decl(TSNode node);

    /// 检查 token 是否为 C++ 关键字（if/for/while/switch/catch）
    static bool is_keyword(const char *type);

    /// 检查节点或其祖先是否为 ERROR 节点
    static bool in_error_subtree(TSNode node);

    /// 根据规则计算两个 token 之间的空白符
    std::string whitespace_between(const Token &prev, const Token &cur,
                                   const std::string &source);

    // ── 状态 ──
    TSParser *parser_ = nullptr;
};

#endif // MYCPPFORMAT_TS_FORMATTER_HPP
