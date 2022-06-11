#include "lc3.h"



//#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE *file = fopen("/Users/franciscoalvarez/Projects/personal/lc3/lc3asm/tests/t1.asm", "r");

    printf("first read");
    read = getline(&line, &len, file);
    //while((read = getline(&line, &len, file)) != -1) {
        printf("rnum ead %zu:\n", read);
        printf("read %s:\n", line);
    //}
}
//#endif