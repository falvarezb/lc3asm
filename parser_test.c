#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "lc3.h"

void test_add_register(void **state) {
    char asm_instr[] = "ADD R0,R1,R2";
    uint16_t machine_instr = parse_add(asm_instr);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 66);
    assert_int_equal(bytes[1], 16);
}

void test_add_imm5_decimal(void **state) {
    char asm_instr[] = "ADD R0,R1,#13";
    uint16_t machine_instr = parse_add(asm_instr);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 109);
    assert_int_equal(bytes[1], 16);
}

void test_add_imm5_hex(void **state) {
    char asm_instr[] = "ADD R0,R1,xa";
    uint16_t machine_instr = parse_add(asm_instr);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 106);
    assert_int_equal(bytes[1], 16);
}

void test_wrong_register_DR(void **state) {
    char asm_instr[] = "ADD R8,R1,xa";
    uint16_t machine_instr = parse_add(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected register but found R8\n");
}

void test_wrong_register_SR1(void **state) {
    char asm_instr[] = "ADD R0,SR1,xa";
    uint16_t machine_instr = parse_add(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected register but found SR1\n");
}

void test_wrong_imm5_too_big_dec(void **state) {
    char asm_instr[] = "ADD R0,R1,#16";
    uint16_t machine_instr = parse_add(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "value of operand imm5 16 is outside the range [-16,15]\n");
}

void test_wrong_imm5_too_small_dec(void **state) {
    char asm_instr[] = "ADD R0,R1,#-17";
    uint16_t machine_instr = parse_add(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "value of operand imm5 -17 is outside the range [-16,15]\n");
}

void test_wrong_imm5_too_big_hex(void **state) {
    char asm_instr[] = "ADD R0,R1,xf1";
    uint16_t machine_instr = parse_add(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "value of operand imm5 f1 is outside the range [-16,15]\n");
}

void test_wrong_imm5_too_small_hex(void **state) {
    char asm_instr[] = "ADD R0,R1,x-f2";
    uint16_t machine_instr = parse_add(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "value of operand imm5 -f2 is outside the range [-16,15]\n");
}

void test_wrong_imm5_format(void **state) {
    char asm_instr[] = "ADD R0,R1,0";
    uint16_t machine_instr = parse_add(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "operand imm5 0 must be decimal or hex\n");
}

void test_wrong_add_instruction(void **state) {
    char asm_instr[] = "AND R0,R1,R2";
    uint16_t machine_instr = parse_add(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected ADD but found AND\n");
}

void test_wrong_element_in_instruction(void **state) {
    char asm_instr[] = "ADD R0,R1,R2,R3";
    uint16_t machine_instr = parse_add(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "unexpected token in ADD instruction\n");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add_register),
        cmocka_unit_test(test_add_imm5_decimal),
        cmocka_unit_test(test_add_imm5_hex),
        cmocka_unit_test(test_wrong_register_DR),
        cmocka_unit_test(test_wrong_register_SR1),
        cmocka_unit_test(test_wrong_imm5_too_big_dec),
        cmocka_unit_test(test_wrong_imm5_too_small_dec),
        cmocka_unit_test(test_wrong_imm5_too_big_hex),
        cmocka_unit_test(test_wrong_imm5_too_small_hex),
        cmocka_unit_test(test_wrong_imm5_format),
        cmocka_unit_test(test_wrong_add_instruction),
        cmocka_unit_test(test_wrong_element_in_instruction)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
