#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_not_register(void  __attribute__((unused)) **state) {
    uint16_t machine_instr;
    parse_not("R4", "R5", &machine_instr, 0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 127);
    assert_int_equal(bytes[1], 153);
}

void test_not_wrong_register_DR(void  __attribute__((unused)) **state) {
    uint16_t machine_instr;
    exit_t result = parse_not("R8", "R1", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Expected register but found R8");
}

void test_not_wrong_register_SR(void  __attribute__((unused)) **state) {
    uint16_t machine_instr;
    exit_t result = parse_not("R0", "SR1", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Expected register but found SR1");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_not_register),
        cmocka_unit_test(test_not_wrong_register_DR),
        cmocka_unit_test(test_not_wrong_register_SR)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
