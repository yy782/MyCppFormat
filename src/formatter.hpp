#ifndef MYCPPFORMAT_FORMATTER_HPP
#define MYCPPFORMAT_FORMATTER_HPP

#include <string>
#include <vector>

class Formatter {
public:
    /// 对 C++ 源码应用所有格式规则，返回格式化后的文本
    std::string format(const std::string &source);

    /// 规范1: 指针/引用对齐 — * 和 & 紧贴变量名（单元测试入口）
    std::string fixPointerAlignment(const std::string &source);

    /// 规范2: 逗号分隔 — 逗号后面不加空格（单元测试入口）
    std::string fixCommaSpacing(const std::string &source);

private:
    // ---- 敏感区域保护 ----
    std::string protect(const std::string &source);
    std::string restore(const std::string &source);
    std::vector<std::string> placeholders_;

    // ---- 内部规则（操作已保护文本）----
    std::string applyPointerAlignment(const std::string &protected_source);
    std::string applyCommaSpacing(const std::string &protected_source);
};

#endif // MYCPPFORMAT_FORMATTER_HPP
