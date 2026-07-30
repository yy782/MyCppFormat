#include <iostream>
#include <vector>
#include <map>
#include <string>

int add(int a,int b,int c) {
    return a + b + c;
}

void print(const std::string &msg,int count) {
    for (int i = 0;i < count;++i) {
        std::cout << msg << "," << i << std::endl;
    }
}

int main() {
    // 逗号在函数调用参数中
    int result = add(1,2,3);

    // 逗号在声明中
    int x,y,z;
    x = 1;
    y = 2;
    z = 3;

    // 逗号在模板参数中
    std::map<int,std::string> m;
    m[1] = "one";

    // 逗号在初始化列表中
    std::vector<int> vec = {10,20,30,40};

    // 注释中的逗号不应被修改
    // hello, world, test
    /* block, comment, test */

    // 字符串中的逗号不应被修改
    const char *msg = "hello, world";

    // 逗号已正确的场景
    print("sum",result);
    print("count",vec.size());

    return 0;
}
