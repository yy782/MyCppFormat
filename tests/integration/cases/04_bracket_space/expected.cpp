#include <iostream>
#include <vector>

// ============================================================
// 规范5: 括号内空格 — 去除括号内首尾空格
// 输入: if ( x ) → 输出: if (x)
// ============================================================

void func(int a,int b ) {
    if (a > 0) {
        std::cout << "positive" << std::endl;
    }

    for (int i = 0 ;i < 10 ;i++) {
        if ((a + i) % 2 == 0) {
            continue;
        }
    }

    int n = (a + b) * ( a - b );
    while (n > 0) {
        n--;
    }

    // 嵌套括号
    int result = (( (a + b) * 2 ) + ( b - a ));
    if (((result > 0))) {
        result /= (2 + 3);
    }
}

int main() {
    func( 3,5 );
    return 0;
}
