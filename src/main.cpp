#include "formatter.hpp"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

static void print_usage(const char *prog_name) {
    std::cerr << "用法: " << prog_name << " <filename.cpp>" << std::endl;
    std::cerr << "说明: 直接格式化 C++ 源文件，原文件备份为 filename.cpp.bak"
              << std::endl;
}

/// 读取文件全部内容
static std::string read_file(const std::string &path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "错误: 无法打开文件 " << path << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::ostringstream oss;
    oss << in.rdbuf();
    return oss.str();
}

/// 将内容写入文件
static void write_file(const std::string &path, const std::string &content) {
    std::ofstream out(path);
    if (!out.is_open()) {
        std::cerr << "错误: 无法写入文件 " << path << std::endl;
        std::exit(EXIT_FAILURE);
    }
    out << content;
}

/// 复制文件（用于备份）
static void copy_file(const std::string &src, const std::string &dst) {
    std::ifstream in(src, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "错误: 无法读取源文件 " << src << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::ofstream out(dst, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "错误: 无法创建备份文件 " << dst << std::endl;
        std::exit(EXIT_FAILURE);
    }
    out << in.rdbuf();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const std::string filepath = argv[1];
    const std::string backup_path = filepath + ".bak";

    // 1. 读取原文件
    std::string source = read_file(filepath);

    // 2. 创建备份
    copy_file(filepath, backup_path);
    std::cout << "已备份: " << backup_path << std::endl;

    // 3. 格式化
    Formatter fmt;
    std::string result = fmt.format(source);

    // 4. 写回原文件
    write_file(filepath, result);
    std::cout << "已格式化: " << filepath << std::endl;

    return EXIT_SUCCESS;
}
