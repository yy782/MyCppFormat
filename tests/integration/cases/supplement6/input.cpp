#include "aws.h"
inline int *ptr = nullptr;

thread_local int *thread_ptr = nullptr;

inline int getMax(int a,int b,int c) {return a > b ? (a > c ? a : c) : (b > c ? b : c);}
thread_local AWS* AWS::_validInstance = nullptr; 

int main () {

    const char *paths[] = {"/get","/ip"," /uuid"};

    char *p[] = {"/get","/ip"," /uuid"};
    auto *a = new int[3];
    auto &b = *a;
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    delete[] a;

    return 0;
}