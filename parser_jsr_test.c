#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "lc3.h"
#include "lc3test.h"

void test_jsr_right_instruction(void **state) {
    char asm_instr[] = "JSR 1";
    uint16_t machine_instr = parse_jsr(asm_instr);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 72);
}

void test_jsr_PCoffset11_too_big(void **state) {
    char asm_instr[] = "JSR 2000";
    uint16_t machine_instr = parse_jsr(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "value of PCoffset11 2000 is outside the range [-1024, 1023]\n");
}

void test_jsr_PCoffset11_too_small(void **state) {
    char asm_instr[] = "JSR -2000";
    uint16_t machine_instr = parse_jsr(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "value of PCoffset11 -2000 is outside the range [-1024, 1023]\n");
}

void test_jsr_wrong_instruction(void **state) {
    char asm_instr[] = "ADD 1";
    uint16_t machine_instr = parse_jsr(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected JSR but found ADD\n");
}

void test_jsr_wrong_element_in_instruction(void **state) {
    char asm_instr[] = "JSR 1 R";
    uint16_t machine_instr = parse_jsr(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "unexpected token in JSR instruction\n");
}

void test_jsr_non_numeric_label_in_instruction(void **state) {
    char asm_instr[] = "JSR A";
    uint16_t machine_instr = parse_jsr(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "value of PCoffset11 A is not a numeric value\n");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_jsr_right_instruction),
        cmocka_unit_test(test_jsr_PCoffset11_too_big),
        cmocka_unit_test(test_jsr_PCoffset11_too_small),     
        cmocka_unit_test(test_jsr_wrong_instruction),
        cmocka_unit_test(test_jsr_wrong_element_in_instruction),
        cmocka_unit_test(test_jsr_non_numeric_label_in_instruction)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
