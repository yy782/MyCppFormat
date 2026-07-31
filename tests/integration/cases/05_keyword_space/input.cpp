#include <cstdio>
#include <cstring>

int helper(int x, int y) {
    return x + y;
}

int main(int argc , char** argv) {
    int a = 1 , b = 2;

    // if without space before '('
    if(a > 0){
        printf("positive\n");
    } else if(b > 0){
        printf("also positive\n");
    }

    // for without space
    for(int i = 0 ;i < 3 ;i++){
        printf("%d\n" , i);
    }

    // while without space
    int n = 3;
    while(n-- > 0){
        printf("looping\n");
    }

    // switch without space
    switch(a){
        case 1:
            printf("one\n");
            break;
        default:
            printf("other\n");
    }

    // nested if
    if((a + b) > 0){
        int result = helper(a , b);
        printf("result: %d\n" , result);
    }

    // string literal should not be modified
    const char* msg = "if(you see this) it's fine";

    return 0;
}
