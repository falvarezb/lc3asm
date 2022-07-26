#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_not_register(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R4", "R5"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3};
    parse_not(&line_metadata);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 127);
    assert_int_equal(bytes[1], 153);
}

void test_not_wrong_register_DR(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R8", "R5"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3, .line_number = 1};
    exit_t result = parse_not(&line_metadata); 

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Expected register but found R8");
}

void test_not_wrong_register_SR(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "SR1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3, .line_number = 1};
    exit_t result = parse_not(&line_metadata); 

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Expected register but found SR1");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_not_register),
        cmocka_unit_test(test_not_wrong_register_DR),
        cmocka_unit_test(test_not_wrong_register_SR)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
