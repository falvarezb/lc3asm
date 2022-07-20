#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_add_SR2(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_add("R0","R1","R2",&machine_instr);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 66);
    assert_int_equal(bytes[1], 16);
}

void test_add_imm5_decimal(void  __attribute__ ((unused)) **state) {  
    uint16_t machine_instr;  
    parse_add("R0","R1","#13",&machine_instr);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 109);
    assert_int_equal(bytes[1], 16);
}

void test_add_imm5_hex(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr; 
    parse_add("R0","R1","xa",&machine_instr);
    char *bytes = (char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 106);
    assert_int_equal(bytes[1], 16);
}

void test_add_wrong_register_DR(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_add("R8","R1","xa",&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "expected register but found R8\n");
}

void test_add_wrong_register_SR1(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr; 
    exit_t result = parse_add("R0","SR1","xa",&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "expected register but found SR1\n");
}

void test_add_wrong_imm5_too_big_dec(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_add("R0","R1","#16",&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "immediate operand (16) outside of range (-16 to 15)");
}

void test_add_wrong_imm5_too_small_dec(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_add("R0","R1","#-17",&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "immediate operand (-17) outside of range (-16 to 15)");
}

void test_add_wrong_imm5_too_big_hex(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_add("R0","R1","xf1",&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "immediate operand (f1) outside of range (-16 to 15)");
}

void test_add_wrong_imm5_too_small_hex(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_add("R0","R1","x-f2",&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "immediate operand (-f2) outside of range (-16 to 15)");
}

void test_add_wrong_imm5_format(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr ;
    exit_t result = parse_add("R0","R1","0",&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "immediate 0 must be decimal or hex");
}

void test_add_wrong_imm5_number(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_add("R0","R1","#y",&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "immediate #y is not a numeric value");
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
