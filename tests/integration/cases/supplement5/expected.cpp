#include <iostream>
#include <string>

// 基础版本：处理最后一个参数（递归终止条件）
void printAll() {
    // 空函数，什么都不做
}

// 可变参数模板函数
template<typename T,typename... Args>
void printAll(T first,Args &&...rest) {
    std::cout << first << " ";
    printAll(rest...);  // 递归调用，处理剩余参数
}

// 使用示例
int main() {
    printAll(1,2.5,"hello",'a');
    // 输出: 1 2.5 hello a
    return 0;
}