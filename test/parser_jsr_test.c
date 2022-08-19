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

void test_jsr_right_no_hash_symbol(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_jsr(&line_metadata);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 72);
}

void test_jsr_right_with_hash_symbol(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "#1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_jsr(&line_metadata);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 72);
}

void test_jsr_PCoffset11_too_big(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "2000"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1};
    exit_t result = parse_jsr(&line_metadata);    
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of offset 2000 is outside the range [-1024, 1023]");
}

void test_jsr_PCoffset11_too_small(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "-2000"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1};
    exit_t result = parse_jsr(&line_metadata);    
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of offset -2000 is outside the range [-1024, 1023]");
}


void test_jsr_with_label(void __attribute__ ((unused))  **state) {    
    add("LABEL", 3);    
    char *tokens[] = {"DOES NOT MATTER", "LABEL"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .instruction_location = 1};
    parse_jsr(&line_metadata);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 72);
}

void test_jsr_non_existent_label(void __attribute__ ((unused))  **state) {      
    char *tokens[] = {"DOES NOT MATTER", "NON_EXISTENT_LABEL"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1};
    exit_t result = parse_jsr(&line_metadata);    
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Symbol not found ('NON_EXISTENT_LABEL')");
}

int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused)) *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_jsr_right_no_hash_symbol, setup, teardown),
        cmocka_unit_test_setup_teardown(test_jsr_right_with_hash_symbol, setup, teardown),
        cmocka_unit_test_setup_teardown(test_jsr_PCoffset11_too_big, setup, teardown),
        cmocka_unit_test_setup_teardown(test_jsr_PCoffset11_too_small, setup, teardown),             
        cmocka_unit_test_setup_teardown(test_jsr_with_label, setup, teardown),
        cmocka_unit_test_setup_teardown(test_jsr_non_existent_label, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
