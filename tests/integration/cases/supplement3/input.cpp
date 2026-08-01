// ============================================================================
// 这是一个用于测试 MyCppFormat 格式化的复杂 C++ 文件
// 包含：模板、宏、条件编译、原始字符串、Unicode、各种语法结构
// ============================================================================

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <type_traits>
#include <string_view>
#include <functional>

// ============================================================================
// 测试规范 2 & 3：逗号和分号（宏体内不应被格式化）
// ============================================================================

#define VECTOR_FOR_EACH(vec, callback) \
    for (size_t i = 0; i < vec.size(); ++i) { \
        callback(vec[i]); \
    }

// 函数式宏 - 形参列表中的逗号不应被格式化（当前限制：会被格式化，属于可接受行为）
#define MAKE_PAIR(a, b) std::make_pair(a, b)
#define MAX_OF(a, b) ((a) > (b) ? (a) : (b))

// 复杂宏：多个参数，包含续行符
#define DECLARE_FUNCTION_TRAIT(CONST, VOLATILE, NOEXCEPT, OVL_REF, REF) \
    template<typename T> \
    struct FunctionTrait<CONST, VOLATILE, NOEXCEPT, OVL_REF, REF> { \
        static constexpr bool is_const = CONST; \
        static constexpr bool is_volatile = VOLATILE; \
        static constexpr bool is_noexcept = NOEXCEPT; \
        static constexpr bool is_ovl_ref = OVL_REF; \
        static constexpr bool is_ref = REF; \
    };

// 宏体中有代码块 - 不应该被格式化
#define SAFE_DELETE(p) \
    do { \
        if (p != nullptr) { \
            delete p; \
            p = nullptr; \
        } \
    } while (0)

// 多行字符串宏
#define LOG_ERROR(msg) \
    std::cerr << "[ERROR] " << msg << " at " << __FILE__ << ":" << __LINE__ << std::endl

// ============================================================================
// 测试条件编译（#if/#ifdef/#else/#endif 不应受影响，但块内代码应被格式化）
// ============================================================================

#ifdef __cplusplus
extern "C" {
#endif

// 这块代码应该被格式化
void c_api_function ( int *   ptr , double   value ) ;

#ifdef __cplusplus
}
#endif

#if __cplusplus >= 201703L
    #define HAVE_IF_CONSTEXPR 1
    // 这里的代码也应被格式化
    template<typename T>
    auto smart_cast(T* ptr) {
        if constexpr (std::is_pointer_v<T>) {
            return ptr;
        } else {
            return &ptr;
        }
    }
#else
    #define HAVE_IF_CONSTEXPR 0
#endif

// ============================================================================
// 测试规范 1：指针/引用对齐（各种复杂情况）
// ============================================================================

class TestClass {
public:
    // 成员变量指针
    int*   m_ptr;
    double& m_ref;
    int** m_pptr;
    int*&  m_ptr_ref;
    const int* const m_const_ptr;
    
    // 构造函数 - 初始化列表中的指针
    TestClass ( int *   ptr , double &   ref ) 
        : m_ptr ( ptr ) 
        , m_ref ( ref ) 
        , m_pptr ( nullptr ) 
        , m_ptr_ref ( m_ptr ) 
        , m_const_ptr ( nullptr ) 
    {}
    
    // 成员函数 - 指针/引用形参和返回值
    int* getPtr ( ) { return m_ptr; }
    const double& getRef ( ) const { return m_ref; }
    
    // 复杂指针类型
    int** getPPtr ( ) { return m_pptr; }
    int*& getPtrRef ( ) { return m_ptr_ref; }
    
    // 函数指针类型参数（这个比较特殊）
    void setCallback ( void ( *   callback ) ( int   , double   ) ) {
        m_callback = callback;
    }
    
    // 模板成员函数
    template<typename T>
    T* getAs ( ) {
        return static_cast < T * > ( m_ptr ) ;
    }
    
    template<typename T>
    const T& getRefAs ( ) const {
        return static_cast < const T & > ( m_ref ) ;
    }

private:
    void (*m_callback) ( int , double ) ;
};

// 自由函数 - 指针/引用形参
void processData ( const int *   data , size_t   size , double &   result ) {
    for ( size_t i = 0 ; i < size ; ++ i ) {
        result += static_cast < double > ( data [ i ] ) ;
    }
}

// 函数指针类型别名（这个也要测试）
typedef void (*CallbackFunc) ( int   , double   ) ;
using PtrToMember = int TestClass :: * ;

// 返回指针的函数
int* createArray ( size_t   count ) {
    return new int [ count ] ( ) ;
}

// 返回引用
const std::string& getDefaultString ( ) {
    static const std::string s = "default" ;
    return s;
}

// ============================================================================
// 测试规范 2：逗号分隔（各种情况）
// ============================================================================

// 模板参数列表
template<typename T, typename U, typename V = std::vector<T>>
class TemplateClass {
public:
    void multiParam ( T a , U b , V c ) {
        // 函数调用中的逗号
        auto result = std::make_pair ( a , b ) ;
        
        // 初始化列表
        std::vector<int> vec = { 1 , 2 , 3 , 4 , 5 } ;
        
        // 多个变量声明
        int x , y , z ;
        T* p1 , * p2 , * p3 ;
        
        // 条件表达式中的逗号
        for ( int i = 0 , j = 10 ; i < j ; ++ i , -- j ) {
            // 逗号运算符
            auto val = ( x = i , y = j , x + y ) ;
        }
    }
    
    // 多参数模板函数
    template<typename A, typename B, typename C>
    auto combine ( A a , B b , C c ) -> decltype ( a + b + c ) {
        return a + b + c;
    }
};

// ============================================================================
// 测试规范 3：分号分隔（for 循环等）
// ============================================================================

void testForLoops ( ) {
    // 标准 for 循环
    for ( int i = 0 ; i < 10 ; ++ i ) {
        std::cout << i << std::endl ;
    }
    
    // 范围 for 循环（不应改变）
    std::vector<int> m_data = { 1, 2, 3 };
    for ( auto& item : m_data ) {
        item *= 2 ;
    }
    
    // 空语句
    for ( int i = 0 ; i < 10 ; ++ i ) 
        ;
    
    // 多个声明
    for ( int i = 0 , j = 0 ; i < 10 && j < 20 ; ++ i , ++ j ) {
        // 分号在 for 的三个部分中
    }
    
    // while 循环
    int count = 0 ;
    while ( count < 100 ) {
        ++ count ;
    }
    
    // do-while
    do {
        -- count ;
    } while ( count > 0 ) ;
}

// ============================================================================
// 测试规范 4：短函数体
// ============================================================================

class ShortFunctionTest {
public:
    // 单行函数体
    int getValue ( ) { return m_value ; }
    void setValue ( int v ) { m_value = v ; }
    bool isEmpty ( ) { return m_data . empty ( ) ; }
    
    // 这些不应该被改变（多行）
    int compute ( int x , int y ) {
        return x + y ;
    }
    
    // 大括号内有注释的短函数
    int getWithComment ( ) { /* comment */ return m_value ; }
    
    // 空函数体
    void noop ( ) { }
    
    // try-catch 块（不是简单函数体）
    int safeGet ( ) try {
        return m_value ;
    } catch ( ... ) {
        return - 1 ;
    }
    
    // 初始化列表 + 函数体
    ShortFunctionTest ( int v ) : m_value ( v ) { }
    
    // 有属性/说明符
    [[nodiscard]] int getConst ( ) const { return m_value ; }
    
private:
    int m_value ;
    std::vector<int> m_data ;
};

// ============================================================================
// 测试规范 5 & 6：括号内空格 + 关键字空格
// ============================================================================

void testControlFlow ( int value , const std::vector<int>& vec ) {
    // if 语句
    if ( value > 0 ) {
        std::cout << "positive" << std::endl ;
    } else if ( value < 0 ) {
        std::cout << "negative" << std::endl ;
    } else {
        std::cout << "zero" << std::endl ;
    }
    
    // if 不带大括号
    if ( value == 0 ) return ;
    
    // switch
    switch ( value ) {
        case 1 :
            std::cout << "one" << std::endl ;
            break ;
        case 2 :
            std::cout << "two" << std::endl ;
            break ;
        default :
            std::cout << "other" << std::endl ;
    }
    
    // while
    int i = 0 ;
    while ( i < 10 ) {
        ++ i ;
    }
    
    // do-while
    do {
        -- i ;
    } while ( i > 0 ) ;
    
    // range-based for
    for ( const auto& item : vec ) {
        std::cout << item << std::endl ;
    }
    
    // try-catch
    try {
        throw std::runtime_error ( "error" ) ;
    } catch ( const std::exception& e ) {
        std::cerr << e . what ( ) << std::endl ;
    } catch ( ... ) {
        std::cerr << "unknown" << std::endl ;
    }
}

// ============================================================================
// 测试字符串字面量和注释（不应被修改）
// ============================================================================

void testStringLiterals ( ) {
    // 普通字符串 - 里面的指针符号不能被修改
    const char* str1 = "int *a; double &b;";
    const char* str2 = "for (int i = 0; i < 10; ++i)";
    
    // 包含逗号和分号的字符串
    const char* csv = "a,b,c,d,e";
    const char* code = "void func(int a, int b);";
    
    // 原始字符串字面量
    const char* raw = R"(int *a = new int;
double &b = *a;
if (x > 0) { return true; })";
    
    // Unicode 字符串
    const char16_t* u16 = u"int *ptr;";
    const char32_t* u32 = U"double &ref;";
    const wchar_t* wide = L"char** argv;";
    
    // 字符串拼接
    const char* concat = "Hello " "int *" " world";
    
    // 行注释中的代码不应被修改
    // int *a;  double &b;  for (int i = 0; i < 10; ++i)
    
    /* 
     * 块注释中的代码也不应被修改
     * int *a = nullptr;
     * double &b = *a;
     * if (x > 0) { return true; }
     */
    
    // 注释中的反斜杠（不应被认为是续行符）
    // this is a backslash: \
    // and this is still a comment
}

// ============================================================================
// 复杂模板和现代 C++ 特性
// ============================================================================

template<typename T, typename... Args>
class ComplexTemplate {
public:
    // 完美转发
    template<typename U>
    void set ( U&& value ) {
        m_data = std::forward<U> ( value ) ;
    }
    
    // 变参模板
    template<typename... Us>
    void setMultiple ( Us&&... values ) {
        ( m_data . push_back ( std::forward<Us> ( values ) ) , ... ) ;
    }
    
    // 返回类型推导
    auto get ( ) const -> const T& {
        return m_data ;
    }
    
    // lambda 表达式（内部不应被格式化）
    auto getProcessor ( ) {
        return [ this ] ( T value ) -> T {
            return m_data + value ;
        } ;
    }
    
    // 静态断言
    static_assert ( std::is_copy_constructible_v<T> , "T must be copy constructible" ) ;
    
private:
    T m_data ;
};

// ============================================================================
// 运算符重载
// ============================================================================

class Vector3D {
public:
    double x , y , z ;
    
    Vector3D ( double x = 0 , double y = 0 , double z = 0 ) 
        : x ( x ) , y ( y ) , z ( z ) { }
    
    // 运算符重载 - 指针/引用
    Vector3D& operator+= ( const Vector3D& other ) {
        x += other . x ;
        y += other . y ;
        z += other . z ;
        return * this ;
    }
    
    Vector3D operator+ ( const Vector3D& other ) const {
        return Vector3D ( x + other . x , y + other . y , z + other . z ) ;
    }
    
    // 一元运算符
    Vector3D operator- ( ) const {
        return Vector3D ( -x , -y , -z ) ;
    }
    
    // 下标运算符
    double& operator[] ( size_t index ) {
        return ( &x ) [ index ] ;
    }
    
    const double& operator[] ( size_t index ) const {
        return ( &x ) [ index ] ;
    }
    
    // 类型转换
    operator double* ( ) {
        return &x ;
    }
    
    operator const double* ( ) const {
        return &x ;
    }
};

// ============================================================================
// 友元函数和类
// ============================================================================

class FriendClass {
    friend void friendFunction ( FriendClass& , int   ) ;
    friend class FriendOfFriend ;
    
public:
    FriendClass ( int   value ) : m_value ( value ) { }
    
private:
    int   m_value ;
};

void friendFunction ( FriendClass& obj , int   value ) {
    obj . m_value = value ;
}

// ============================================================================
// 内联汇编和属性
// ============================================================================

#if defined(__GNUC__) || defined(__clang__)
[[deprecated ( "use newFunction instead" ) ]] 
void oldFunction ( ) {
    // deprecated function
}

[[noreturn]] void throwError ( ) {
    throw std::runtime_error ( "error" ) ;
}

[[gnu::always_inline]] 
inline int fastAdd ( int a , int b ) {
    return a + b ;
}

void testAttributes ( ) {
    // Test deprecated function
    oldFunction ( ) ;
    // Test always_inline
    int result = fastAdd ( 1 , 2 ) ;
    ( void ) result ;
}
#endif

// ============================================================================
// 模板特化和偏特化
// ============================================================================

template<typename T>
struct TypeTraits {
    static constexpr bool is_integral = false ;
    static constexpr bool is_floating = false ;
    static constexpr const char* name = "unknown" ;
} ;

template<>
struct TypeTraits<int> {
    static constexpr bool is_integral = true ;
    static constexpr bool is_floating = false ;
    static constexpr const char* name = "int" ;
} ;

template<>
struct TypeTraits<double> {
    static constexpr bool is_integral = false ;
    static constexpr bool is_floating = true ;
    static constexpr const char* name = "double" ;
} ;

// 偏特化
template<typename T>
struct TypeTraits<T*> {
    static constexpr bool is_pointer = true ;
    using pointee_type = T ;
} ;

// ============================================================================
// 嵌套类和匿名命名空间
// ============================================================================

namespace {
    // 匿名命名空间中的代码也应被格式化
    class InternalHelper {
    public:
        static int* process ( int*   data , size_t   size ) {
            for ( size_t i = 0 ; i < size ; ++ i ) {
                data [ i ] *= 2 ;
            }
            return data ;
        }
    } ;
    
    // 全局变量（指针）
    int* g_global_ptr = nullptr ;
    double& g_global_ref = * ( double * ) malloc ( sizeof ( double ) ) ;
}

// ============================================================================
// 主函数
// ============================================================================

int main ( int   argc , char**   argv ) {
    // 测试各种功能
    TestClass test ( new int ( 42 ) , * new double ( 3.14 ) ) ;
    
    // 指针/引用声明
    int* ptr = test . getPtr ( ) ;
    double ref = test . getRef ( ) ;
    int** pptr = test . getPPtr ( ) ;
    int*& ptr_ref = test . getPtrRef ( ) ;
    
    // 逗号和分号
    for ( int i = 0 ; i < 10 ; ++ i ) {
        std::cout << i << ", " << i * i << std::endl ;
    }
    
    // 控制流
    if ( ptr != nullptr ) {
        processData ( ptr , 10 , ref ) ;
    } else {
        std::cerr << "null pointer" << std::endl ;
    }
    
    // 模板使用
    TemplateClass<int, double, std::vector<int>> tc ;
    tc . multiParam ( 42 , 3.14 , { 1 , 2 , 3 } ) ;
    
    auto result = tc . combine ( 1 , 2.5 , 3.14f ) ;
    
    // Vector3D 测试
    Vector3D v1 ( 1.0 , 2.0 , 3.0 ) ;
    Vector3D v2 ( 4.0 , 5.0 , 6.0 ) ;
    Vector3D v3 = v1 + v2 ;
    
    // Lambda 表达式（内部不应被格式化）
    auto lambda = [ & ptr , & ref ] ( int   multiplier ) -> int {
        return ( * ptr ) * multiplier + static_cast < int > ( ref ) ;
    } ;
    
    std::cout << lambda ( 10 ) << std::endl ;
    
    // 字符串字面量（不应被修改）
    std::cout << "int *a; double &b;" << std::endl ;
    std::cout << R"(for (int i = 0; i < 10; ++i))" << std::endl ;
    
    // 宏调用（宏体内的空格应保持不变）
    auto vec = std::vector<int> { 1 , 2 , 3 } ;
    VECTOR_FOR_EACH ( vec , [ ] ( int   x ) {
        std::cout << x << std::endl ;
    } )
    
    auto pair = MAKE_PAIR ( "hello" , 42 ) ;
    int max_val = MAX_OF ( 100 , 200 ) ;
    
    // 类型特征
    static_assert ( TypeTraits<int>::is_integral , "int should be integral" ) ;
    static_assert ( TypeTraits<double>::is_floating , "double should be floating" ) ;
    
    // 条件编译块
    #ifdef HAVE_IF_CONSTEXPR
        auto casted = smart_cast ( ptr ) ;
        std::cout << typeid ( casted ) . name ( ) << std::endl ;
    #endif
    
    // 清理
    SAFE_DELETE ( ptr ) ;
    delete &ref ;
    
    return 0 ;
}

// 文件末尾注释 - 不应被修改
// int *last_ptr = nullptr;
// double &last_ref = *last_ptr;