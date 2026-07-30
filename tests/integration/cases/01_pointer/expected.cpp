#include <iostream>
#include <vector>
#include <string>

void process(int *data,double &value);
void transform(const char **args,int &count);
char *createBuffer(size_t *size);

int main() {
    int    value = 42;
    int *a = &value;
    int *b = &value;
    double  num = 3.14;
    double &c = num;
    double &d = num;

    void *ptr = nullptr;
    const char *msg = "hello int* a;";
    char **argv;

    int x,*p = &value,*q = &value;
    int *&ref = a;

    std::vector<int*> vec;

    // comment: int* a; double & b;
    int *result = 0;
    return 0;
}
