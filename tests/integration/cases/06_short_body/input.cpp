#include <iostream>
#include <vector>

// ============================================================
// 规范4: 短函数体 — 单行函数体内大括号去除首尾空格
// 输入: { return x; } → 输出: {return x;}
// 仅对函数体生效，不影响 if/switch/try-catch
// ============================================================

// 单行函数体 — 含空格
int simple_func() { return 42; }

int add( int a, int b ) { return a + b; }

// 空函数体
void empty_func() { }

// 纯分号函数体 — 分号规则也生效
void null_stmt() { ; }

// 单声明函数体
int declared_var() { int x = 0; }

// 复杂表达式函数体
double calc( double x, double y ) { return ( x + y ) * ( x - y ); }

// 已紧凑 — 应保持不变
int already_compact() {return 0;}

// 指针返回类型 + 短函数体
int * get_ptr() { return nullptr; }

// 引用返回类型 + 短函数体
int & get_ref() { static int x = 0; return x; }

// 多行函数体 — 短函数体规则不应影响
int multi_line( int n ) {
    int result = 0;
    for ( int i = 0; i < n; i++ ) {
        result += i;
    }
    return result;
}

// 含 if 的函数体 — if 的大括号不应受影响
void func_with_if( int x ) {
    if ( x > 0 ) { return; }
}

// 含 switch 的函数体
void func_with_switch( int x ) {
    switch ( x ) { case 1: return; }
}

// 含 try-catch 的函数体
void func_with_catch() {
    try { } catch ( ... ) { }
}

// 嵌套大括号的短函数体
int nested_func() { { int x = 0; } }

// braced initializer — 不应被短函数体规则压缩
// 直接父节点是 initializer_list 而非 compound_statement
int return_braced() { return { 1 }; }

std::vector<int> return_braced_list() { return { 1, 2, 3 }; }

std::vector<int> make_vec() { return { 1, 2, 3, 4 }; }

// 类成员函数风格
struct Test {
    int get_val() { return val_; }
    void set_val( int v ) { val_ = v; }
    int val_;
};

int main() {
    simple_func();
    return 0;
}
