// ============================================================
// supplement4: 综合集成测试
// 覆盖: operator重载 / lambda捕获 / 构造函数初始化列表
//        else if多分支 / 嵌套模板
// ============================================================

// ============================================================
// 1. operator 运算符重载
// ============================================================

// 1.1 基础二元运算符声明（含多余空格）
int  operator+(int a,  int b) { return a + b; }
bool operator==(const MyClass  &  lhs, const MyClass& rhs) { return lhs.x  ==  rhs.x; }

// 1.2 赋值运算符（返回值含引用）
MyClass  & operator=(const MyClass  & other) { x = other.x;  return *this ; }

// 1.3 转换运算符
operator bool ()  const { return x != 0; }

// 1.4 下标运算符
int  & operator[]( size_t  index) { return data[ index ]; }

// 1.5 operator new / delete
void  *  operator new( size_t  size) { return malloc(size ); }
void  operator delete( void*  ptr) noexcept { free(ptr ); }

// 1.6 调用运算符
void  operator ( ) () { }

// 1.7 运算符重载正常使用（不应被格式化影响）
int  use_operator() {
    int  x = a + b;
    int  y = a * b;   // * 是乘法，不是指针标记
    int  z = a - b;
    bool  eq = (x == y);
    return x ;
}

// ============================================================
// 2. lambda 表达式捕获列表
// ============================================================

// 2.1 基础值捕获
auto l1 = [a , b , c]( int x) { return a + b + c + x ; };

// 2.2 引用捕获
auto l2 = [ &a , &b]( int x) { return a + b + x ; };

// 2.3 混合捕获（= 默认 + 特定引用）
auto l3 = [= ,  &a ,  &b]( int x) { return a + b + x ; };

// 2.4 初始化捕获
auto l4 = [x = std::move( a) , y =  calc( b )]( ) { return x + y ; };

// 2.5 泛型 lambda（C++14）
auto l5 = []( auto  a , auto  b) { return a + b ; };

// 2.6 lambda 模板（C++20）
auto l6 = []<typename  T>( T  a , T  b) { return a + b; };

// 2.7 空捕获和无捕获
auto l7 = []( ) { return 42 ; };
auto l8 = [ x ]( ) { return x ; };

// 2.8 嵌套 lambda
auto l9 = [a]( int x) {
    return [a , x]( int y) { return a + x + y ; };
};

// 2.9 捕获列表含逗号 + 函数体内含逗号
auto l10 = [a , b , c]( int  x , int  y) { return func( a , b , x , y ); };

// ============================================================
// 3. 构造函数初始化列表
// ============================================================

struct ConstructorTest {
    // 3.1 基础初始化列表（含多余空格/逗号空格）
    ConstructorTest(int  a , int  b)
        : x_( a ) , y_( b) , z_( a + b) {}

    // 3.2 带指针/引用成员的初始化列表
    ConstructorTest(int* p , const std::string  & s)
        : ptr_( p ) , str_( s ) {}

    // 3.3 委托构造函数
    ConstructorTest() : ConstructorTest( 0 , 0 ) {}

    // 3.4 初始化列表中含函数调用
    ConstructorTest(double d)
        : x_( static_cast<int>( d ) ) , y_( calc( d ) ) {}

    // 3.5 成员函数定义含分号换行
    void foo() {
        int a = 1 ; int b = 2 ;
        bar( a , b );
    }

private:
    int x_;
    int y_;
    int z_;
    int* ptr_;
    std::string str_;
};

// 3.6 继承 + 初始化列表
struct Derived : public ConstructorTest {
    Derived(int a , double d)
        : ConstructorTest( a , static_cast<int>( d) ) , extra_( d ) {}
private:
    double extra_;
};

// ============================================================
// 4. else if 多分支条件
// ============================================================

// 4.1 基础 if / else if / else
void test_if_chain(int x) {
    if ( x == 0 ) { return ; }
    else if ( x == 1 ) { return ; }
    else if ( x == 2 ) { return ; }
    else { return ; }
}

// 4.2 else if 紧贴（无空格）
void test_tight_else_if(int x) {
    if(x == 0){return;}
    else if(x == 1){return;}
    else if(x == 2){return;}
    else{return;}
}

// 4.3 嵌套 if-else
void test_nested_if(int x , int y) {
    if ( x > 0 ) {
        if ( y > 0 ) { return ; }
        else if ( y < 0 ) { return ; }
    }
    else if ( x < 0 ) {
        if ( y > 0 ) { return ; }
        else { return ; }
    }
    else { return ; }
}

// 4.4 for / while / switch 关键字间距
void test_control_keywords(int n) {
    // for 关键字 + 括号空格
    for( int  i = 0 ; i < n ; ++i) { continue ; }
    // while 关键字 + 括号空格
    while( n  > 0 ) { --n ; }
    // switch 关键字 + 括号空格
    switch( n ) {
        case 0 : break ;
        case 1 : break ;
        default : break ;
    }
    // do-while
    do{ --n ; } while( n  > 0 );

    // catch（在 try-catch 块内）
    try { throw 1; }
    catch( int e) { }
    catch( ...) { }
}

// ============================================================
// 5. 嵌套模板（逗号处理 + 双右尖括号 >>）
// ============================================================

// 5.1 基础嵌套模板
std::map< int , std::vector< int > > m1;

// 5.2 双右尖括号（C++11 起合法，tree-sitter 需正确解析）
std::map<int, std::vector<int*>> m2;

// 5.3 深嵌套 + 指针/引用
std::map< int , std::map< int , std::set< int*  > > > m3;

// 5.4 模板 + 初始化
std::vector< std::pair< int , double > > vec = { {1 , 2.0} , {3 , 4.0} };

// 5.5 模板函数声明/定义
template< typename  T , typename  U >
auto  make_pair( T&&  t , U&&  u ) -> std::pair< T , U > {
    return std::pair< T , U >( std::forward< T >( t) , std::forward< U >( u ) );
}

// 5.6 模板特化/偏特化
template< typename  T >
struct  MyTraits< std::vector< T > > {
    using  type = typename  std::vector< T >::value_type;
};

// 5.7 模板默认参数
template< typename  T = int , typename  Alloc = std::allocator< T > >
class  MyContainer { };

// 5.8 函数模板实例化调用
void call_template() {
    auto result = my_func< int , double >( 1 , 2.0 );
    auto v = std::vector< int >{ 1 , 2 , 3 };
}

// 5.9 模板模板参数
template< template< typename , typename > class  Container >
struct  TTAdapter { };
