#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_add_SR2(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","R1","R2");
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 66);
    assert_int_equal(bytes[1], 16);
}

void test_add_imm5_decimal(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","R1","#13");
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 109);
    assert_int_equal(bytes[1], 16);
}

void test_add_imm5_hex(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","R1","xa");
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 106);
    assert_int_equal(bytes[1], 16);
}

void test_add_wrong_register_DR(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R8","R1","xa");
    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected register but found R8\n");
}

void test_add_wrong_register_SR1(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","SR1","xa");
    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected register but found SR1\n");
}

void test_add_wrong_imm5_too_big_dec(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","R1","#16");
    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "immediate 16 is outside the range [-16,15]\n");
}

void test_add_wrong_imm5_too_small_dec(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","R1","#-17");
    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "immediate -17 is outside the range [-16,15]\n");
}

void test_add_wrong_imm5_too_big_hex(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","R1","xf1");
    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "immediate f1 is outside the range [-16,15]\n");
}

void test_add_wrong_imm5_too_small_hex(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","R1","x-f2");
    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "immediate -f2 is outside the range [-16,15]\n");
}

void test_add_wrong_imm5_format(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","R1","0");
    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "immediate 0 must be decimal or hex\n");
}

void test_add_wrong_imm5_number(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr = parse_add("R0","R1","#y");
    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "immediate #y is not a numeric value\n");
}


int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add_SR2),
        cmocka_unit_test(test_add_imm5_decimal),
        cmocka_unit_test(test_add_imm5_hex),
        cmocka_unit_test(test_add_wrong_register_DR),
        cmocka_unit_test(test_add_wrong_register_SR1),
        cmocka_unit_test(test_add_wrong_imm5_too_big_dec),
        cmocka_unit_test(test_add_wrong_imm5_too_small_dec),
        cmocka_unit_test(test_add_wrong_imm5_too_big_hex),
        cmocka_unit_test(test_add_wrong_imm5_too_small_hex),
        cmocka_unit_test(test_add_wrong_imm5_format),
        cmocka_unit_test(test_add_wrong_imm5_number)        
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
