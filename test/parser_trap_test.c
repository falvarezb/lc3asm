#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

static int setup(void **state) {
    clearerrdesc();
    initialize();
    return 0;
}

static int teardown(void **state) {
    initialize();
    return 0;
}

static void test_trap_right_instr(void __attribute__ ((unused)) **state) {  
    char *tokens[] = {"DOES NOT MATTER", "1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};      
    parse_trap(&line_metadata);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 240);
}

static void test_trap_trapvector_too_big(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "300"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1};      
    exit_t result = parse_trap(&line_metadata);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of trapvector 300 is outside the range [0, 255]");
}

static void test_trap_trapvector_too_small(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER","-1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1};      
    exit_t result = parse_trap(&line_metadata);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of trapvector -1 is outside the range [0, 255]");
}


int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused)) *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_trap_right_instr, setup, teardown),
        cmocka_unit_test_setup_teardown(test_trap_trapvector_too_big, setup, teardown),
        cmocka_unit_test_setup_teardown(test_trap_trapvector_too_small, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
