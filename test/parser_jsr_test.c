#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_jsr_right_PCoffset11(void __attribute__ ((unused)) **state) {
    char asm_instr[] = "JSR 1";
    uint16_t machine_instr = parse_jsr(asm_instr, 0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 72);
}

void test_jsr_PCoffset11_too_big(void __attribute__ ((unused))  **state) {
    char asm_instr[] = "JSR 2000";
    uint16_t machine_instr = parse_jsr(asm_instr, 0);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "value of PCoffset11 2000 is outside the range [-1024, 1023]\n");
}

void test_jsr_PCoffset11_too_small(void __attribute__ ((unused))  **state) {
    char asm_instr[] = "JSR -2000";
    uint16_t machine_instr = parse_jsr(asm_instr, 0);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "value of PCoffset11 -2000 is outside the range [-1024, 1023]\n");
}

void test_jsr_wrong_instruction(void __attribute__ ((unused))  **state) {
    char asm_instr[] = "ADD 1";
    uint16_t machine_instr = parse_jsr(asm_instr, 0);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected JSR but found ADD\n");
}

void test_jsr_wrong_element_in_instruction(void __attribute__ ((unused))  **state) {
    char asm_instr[] = "JSR 1 R";
    uint16_t machine_instr = parse_jsr(asm_instr, 0);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "unexpected token in JSR instruction\n");
}

void test_jsr_with_label(void __attribute__ ((unused))  **state) {
    initialize();
    add("LABEL", 0x3003);
    char asm_instr[] = "JSR LABEL";
    uint16_t machine_instr = parse_jsr(asm_instr, 0x3001);

    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 72);
    delete("LABEL");
}

void test_jsr_non_existent_label(void __attribute__ ((unused))  **state) {
    char asm_instr[] = "JSR NON_EXISTENT_LABEL";
    uint16_t machine_instr = parse_jsr(asm_instr, 0);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "Symbol not found ('NON_EXISTENT_LABEL')\n");
}

int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused)) *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_jsr_right_PCoffset11),
        cmocka_unit_test(test_jsr_PCoffset11_too_big),
        cmocka_unit_test(test_jsr_PCoffset11_too_small),     
        cmocka_unit_test(test_jsr_wrong_instruction),
        cmocka_unit_test(test_jsr_wrong_element_in_instruction),
        cmocka_unit_test(test_jsr_with_label),
        cmocka_unit_test(test_jsr_non_existent_label)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
