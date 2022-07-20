#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_and_SR2(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,R2";
    uint16_t machine_instr;
    parse_and(asm_instr, &machine_instr, 0);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 66);
    assert_int_equal(bytes[1], 80);
}

void test_and_imm5_decimal(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,#13";
    uint16_t machine_instr;
    parse_and(asm_instr, &machine_instr, 0);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 109);
    assert_int_equal(bytes[1], 80);
}

void test_and_imm5_hex(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,xa";
    uint16_t machine_instr;
    parse_and(asm_instr, &machine_instr, 0);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 106);
    assert_int_equal(bytes[1], 80);
}

void test_and_wrong_register_DR(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R8,R1,xa";
    uint16_t machine_instr;
    exit_t result = parse_and(asm_instr, &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "expected register but found R8\n");
}

void test_and_wrong_register_SR1(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,SR1,xa";
    uint16_t machine_instr;
    exit_t result = parse_and(asm_instr, &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "expected register but found SR1\n");
}

void test_and_wrong_imm5_too_big_dec(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,#16";
    uint16_t machine_instr;
    exit_t result = parse_and(asm_instr, &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (16) outside of range (-16 to 15)");
}

void test_and_wrong_imm5_too_small_dec(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,#-17";
    uint16_t machine_instr;
    exit_t result = parse_and(asm_instr, &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (-17) outside of range (-16 to 15)");
}

void test_and_wrong_imm5_too_big_hex(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,xf1";
    uint16_t machine_instr;
    exit_t result = parse_and(asm_instr, &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (f1) outside of range (-16 to 15)");
}

void test_and_wrong_imm5_too_small_hex(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,x-f2";
    uint16_t machine_instr;
    exit_t result = parse_and(asm_instr, &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (-f2) outside of range (-16 to 15)");
}

void test_and_wrong_imm5_format(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,0";
    uint16_t machine_instr;
    exit_t result = parse_and(asm_instr, &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate 0 must be decimal or hex");
}

void test_and_wrong_imm5_number(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,#y";
    uint16_t machine_instr;
    exit_t result = parse_and(asm_instr, &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate #y is not a numeric value");
}

void test_and_wrong_element_in_instruction(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,R2,R3";
    uint16_t machine_instr;
    exit_t result = parse_and(asm_instr, &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "unexpected token in AND instruction");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_and_SR2),
        cmocka_unit_test(test_and_imm5_decimal),
        cmocka_unit_test(test_and_imm5_hex),
        cmocka_unit_test(test_and_wrong_register_DR),
        cmocka_unit_test(test_and_wrong_register_SR1),
        cmocka_unit_test(test_and_wrong_imm5_too_big_dec),
        cmocka_unit_test(test_and_wrong_imm5_too_small_dec),
        cmocka_unit_test(test_and_wrong_imm5_too_big_hex),
        cmocka_unit_test(test_and_wrong_imm5_too_small_hex),
        cmocka_unit_test(test_and_wrong_imm5_format),
        cmocka_unit_test(test_and_wrong_imm5_number),
        cmocka_unit_test(test_and_wrong_element_in_instruction)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
