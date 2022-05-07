#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "lc3.h"

void test_not_register(void **state) {
    char asm_instr[] = "NOT R4,R5";
    uint16_t machine_instr = parse_not(asm_instr);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 127);
    assert_int_equal(bytes[1], 153);
}

void test_wrong_register_DR(void **state) {
    char asm_instr[] = "NOT R8,R1";
    uint16_t machine_instr = parse_not(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected register but found R8\n");
}

void test_wrong_register_SR(void **state) {
    char asm_instr[] = "NOT R0,SR1";
    uint16_t machine_instr = parse_not(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected register but found SR1\n");
}

void test_wrong_not_instruction(void **state) {
    char asm_instr[] = "ADD R0,R1";
    uint16_t machine_instr = parse_not(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected NOT but found ADD\n");
}

void test_wrong_element_in_instruction(void **state) {
    char asm_instr[] = "NOT R0,R1,R2,R3";
    uint16_t machine_instr = parse_not(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "unexpected token in NOT instruction\n");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_not_register),
        cmocka_unit_test(test_wrong_register_DR),
        cmocka_unit_test(test_wrong_register_SR),
        cmocka_unit_test(test_wrong_not_instruction),
        cmocka_unit_test(test_wrong_element_in_instruction)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
