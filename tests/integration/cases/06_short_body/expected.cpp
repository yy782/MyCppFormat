#include <cstdio>

int helper(int x) {
    return x * 2;
}

int main() {
    int a = 5;

    // 单行函数体 — 内部空格应被移除
    if (a > 0) {return 1;}
    else if (a == 0) {return 0;}
    else {return -1;}

    // for 循环单行体
    for (int i = 0 ;i < 3 ;i++) {printf("%d\n" ,i);}

    // while 循环
    int n = 2;
    while (n--) {printf("loop\n");}

    // 多行体保留缩进
    if (a > 0) {
        printf("positive\n");
        printf("value: %d\n" ,helper(a));
    }

    // 字符串内的大括号不受影响
    const char *brace_str = "{ inside string }";

    return 0;
}
