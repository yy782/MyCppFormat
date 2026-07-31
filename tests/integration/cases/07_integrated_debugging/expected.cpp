#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

// ============================================================
// 集成调试测试：覆盖所有规则组合的复杂场景
// ============================================================

// ============================================================
// 场景1: 指针/引用/逗号/分号/括号/关键字混合
// ============================================================
void analyze(int *data ,double &value ,const char **meta) ;

int *create_buf(size_t *sz) ;

void transform_val(int &x) {
    if (x > 0) {
        x *= 2 ;
    }
}

int add_ptrs(int *a ,int *b) {return (*a) + (*b) ;}

// ============================================================
// 场景2: 嵌套结构（if/for/while 嵌套、多层大括号）
//   内部 if/for 的大括号不应被短函数体规则压缩
// ============================================================
void nested_control_flow(int n) {
    for (int i = 0 ;i < n ;i++) {
        if (i % 2 == 0) {
            for (int j = 0 ;j < i ;j++) {
                if (j % 3 == 0) { continue ;}
            }
        }
        while (i > 10) {
            i-- ;
        }
    }
}

// ============================================================
// 场景3: 模板、STL 容器
//   <> 不处理，但 () 和 , 规则照常生效
// ============================================================
std::vector<int> filter_data(const std::vector<int> &input ,int threshold) {
    std::vector<int> result ;
    for (size_t i = 0 ;i < input.size() ;i++) {
        if (input[ i ] > threshold) {
            result.push_back(input[ i ]) ;
        }
    }
    return result ;
}

std::map<std::string ,int> build_counter() {return std::map<std::string ,int>() ;}

// ============================================================
// 场景4: 字符串字面量、注释、预处理指令
//   字符串和注释中的内容不应被修改
//   #include / #define 保持原样
// ============================================================
#define MAX_SIZE 1024

// 这个注释包含: int* a ; if ( x ) { return ; } — 不应被格式化
/* 块注释: void f( int x ) { return x ; } */

void string_literal_test() {
    const char *msg = "int* p ; if ( x ) { return ; }" ;
    const char *fmt = "a=%d, b=%d, c=%d\n" ;

    // 行注释也包含: for( int i=0 ; i<10 ; i++ )
    std::cout << msg << std::endl ;
}

// ============================================================
// 场景5: 真实世界代码片段
//   模拟真实项目中的一个类，包含构造函数、成员函数、运算符重载
// ============================================================
class DataProcessor {
    int *buf ;
    size_t  cap ;
    size_t  len ;
public:
    DataProcessor(size_t capacity) : buf(new int[ capacity ]) ,cap(capacity) ,len(0) {}
    ~DataProcessor() {delete[] buf ;}

    bool push(int val) {
        if (len >= cap) { return false ;}
        buf[ len++ ] = val ;
        return true ;
    }

    int &at(size_t idx) {return buf[ idx ] ;}

    int sum() const {
        int total = 0 ;
        for (size_t i = 0 ;i < len ;i++) {
            total += buf[ i ] ;
        }
        return total ;
    }
} ;

int main() {
    // 场景1 验证
    int   a = 42 ,b = 10 ;
    int *p = &a ;
    int *q = &b ;
    transform_val(*p) ;

    // 场景2 验证
    nested_control_flow(5) ;

    // 场景3 验证
    std::vector<int>  data = { 1 ,2 ,3 ,4 ,5 } ;
    std::vector<int>  filtered = filter_data(data ,3) ;

    // 场景4 验证
    string_literal_test() ;

    // 场景5 验证
    DataProcessor  proc(10) ;
    proc.push(100) ;
    proc.push(200) ;

    std::cout << "sum=" << proc.sum() << std::endl ;
    return 0 ;
}
