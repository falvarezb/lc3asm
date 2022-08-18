#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_jmp_register(void  __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R6"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_jmp(&line_metadata, JMP);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 128);
    assert_int_equal(bytes[1], 193);
}

void test_jmp_wrong_register_BaseR(void  __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R8"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1};
    exit_t result = parse_jmp(&line_metadata, JMP);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Expected register but found R8");
}


int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_jmp_register),
        cmocka_unit_test(test_jmp_wrong_register_BaseR)                       
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
