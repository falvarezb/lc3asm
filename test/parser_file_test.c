#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_file(void  __attribute__ ((unused)) **state) {
    FILE * source_file = fopen("/Users/franciscoalvarez/Projects/personal/lc3/lc3asm/test/t1.asm", "r");
    FILE * actual_obj_file = fopen("/Users/franciscoalvarez/Projects/personal/lc3/lc3asm/test/t1.actual.obj", "w");    

    parse_file(source_file, actual_obj_file);
    fclose(source_file);
    fclose(actual_obj_file);

    FILE * expected_obj_file = fopen("/Users/franciscoalvarez/Projects/personal/lc3/lc3asm/test/t1.obj", "r");
    actual_obj_file = fopen("/Users/franciscoalvarez/Projects/personal/lc3/lc3asm/test/t1.actual.obj", "r");
    
    char buf_expected[2];
    char buf_actual[2];
    size_t num_lines = 1;

    size_t read;
    while((read = fread(buf_expected, 1, 2, expected_obj_file)) == 2) {
        read = fread(buf_actual, 1, 2, actual_obj_file);
        printf("line checked: %zu\n", num_lines);
        assert_int_equal(read, 2);
        assert_true(buf_expected[0] == buf_actual[0] && buf_expected[1] == buf_actual[1]);
        num_lines++;       
    }
    
    assert_int_equal(read, fread(buf_actual, 1, 2, actual_obj_file));
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_file)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
