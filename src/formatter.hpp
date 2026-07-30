#ifndef MYCPPFORMAT_FORMATTER_HPP
#define MYCPPFORMAT_FORMATTER_HPP

#include <string>

class Formatter {
public:
    /// 对 C++ 源码应用所有格式规则，返回格式化后的文本
    std::string format(const std::string &source);
};

#endif // MYCPPFORMAT_FORMATTER_HPP
