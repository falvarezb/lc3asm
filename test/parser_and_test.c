#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_and_SR2(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    parse_and("R0","R1","R2", &machine_instr, 0);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 66);
    assert_int_equal(bytes[1], 80);
}

void test_and_imm5_decimal(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    parse_and("R0","R1","#13", &machine_instr, 0);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 109);
    assert_int_equal(bytes[1], 80);
}

void test_and_imm5_hex(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    parse_and("R0","R1","xa", &machine_instr, 0);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 106);
    assert_int_equal(bytes[1], 80);
}

void test_and_wrong_register_DR(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_and("R8","R1","xa", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Expected register but found R8");
}

void test_and_wrong_register_SR1(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_and("R0","SR1","xa", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Expected register but found SR1");
}

void test_and_wrong_imm5_too_big_dec(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_and("R0","R1","#16", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (16) outside of range (-16 to 15)");
}

void test_and_wrong_imm5_too_small_dec(void  __attribute__((unused)) **state) {
    char asm_instr[] = "AND R0,R1,#-17";
    uint16_t machine_instr;
    exit_t result = parse_and("R0","R1","#-17", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (-17) outside of range (-16 to 15)");
}

void test_and_wrong_imm5_too_big_hex(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_and("R0","R1","xf1", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (f1) outside of range (-16 to 15)");
}

void test_and_wrong_imm5_too_small_hex(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_and("R0","R1","x-f2", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (-f2) outside of range (-16 to 15)");
}

void test_and_wrong_imm5_format(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_and("R0","R1","0", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate 0 must be decimal or hex");
}

void test_and_wrong_imm5_number(void  __attribute__((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_and("R0","R1","#y", &machine_instr, 0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate #y is not a numeric value");
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
        cmocka_unit_test(test_and_wrong_imm5_number)        
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
