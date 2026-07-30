#include <iostream>

void func(int *a,double &b,char c) {
    std::cout << "hello; world" << std::endl;
}

// 分号规则测试: for 循环、多条语句、宏保护
#define SEMI_MACRO(x) \
    x;

int main() {
    int *p,*q;
    double val = 3.14;
    double &r = val;

    func(p,r,';');

    for (int i = 0;i < 10;++i) {
        int a;int b;int c;
        a = 1;b = 2;c = 3;
        std::cout << a << b << c << std::endl;
    }

    return 0;
}
