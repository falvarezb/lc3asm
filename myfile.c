
#include <stdio.h>
#include "myfile.h"

char* hello_world() {
    return "hello world";
}

#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {
    printf(hello_world());
}
#endif