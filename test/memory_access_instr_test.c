#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_ld_right_instr(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_pc_relative_addressing_mode("R0","1", 0, &machine_instr,0,LD);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 32);
}

void test_st_right_instr(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_pc_relative_addressing_mode("R0","1", 0, &machine_instr,0,ST);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 48);
}

void test_ldi_right_instr(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_pc_relative_addressing_mode("R0","1", 0, &machine_instr,0,LDI);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 160);
}

void test_sti_right_instr(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_pc_relative_addressing_mode("R0","1", 0, &machine_instr,0,STI);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 176);
}

void test_lea_right_instr(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_pc_relative_addressing_mode("R0","1", 0, &machine_instr,0,LEA);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 224);
}

void test_ld_PCoffset9_wrong_register(void __attribute__ ((unused))  **state) {    
    uint16_t machine_instr;
    exit_t result = parse_pc_relative_addressing_mode("R9","3", 0,&machine_instr,0,LD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Expected register but found R9");
}

void test_ld_PCoffset9_too_big(void __attribute__ ((unused))  **state) {    
    uint16_t machine_instr;
    exit_t result = parse_pc_relative_addressing_mode("R0","300", 0,&machine_instr,0,LD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Value of offset 300 is outside the range [-256, 255]");
}

void test_ld_PCoffset9_too_small(void __attribute__ ((unused))  **state) {    
    uint16_t machine_instr;
    exit_t result = parse_pc_relative_addressing_mode("R0","-300", 0,&machine_instr,0,LD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Value of offset -300 is outside the range [-256, 255]");
}


void test_ld_with_label(void __attribute__ ((unused))  **state) {
    initialize();
    add("LABEL", 0x3003);    
    uint16_t machine_instr;
    parse_pc_relative_addressing_mode("R0","LABEL", 0x3001,&machine_instr,0,LD);

    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 32);
    initialize();
}

void test_ld_non_existent_label(void __attribute__ ((unused))  **state) {
    initialize();    
    uint16_t machine_instr;
    exit_t result = parse_pc_relative_addressing_mode("R0","NON_EXISTENT_LABEL", 0,&machine_instr,0,LD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Symbol not found ('NON_EXISTENT_LABEL')");
}

int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused)) *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_ld_right_instr),
        cmocka_unit_test(test_st_right_instr),
        cmocka_unit_test(test_ldi_right_instr),
        cmocka_unit_test(test_sti_right_instr),
        cmocka_unit_test(test_lea_right_instr),
        cmocka_unit_test(test_ld_PCoffset9_wrong_register),    
        cmocka_unit_test(test_ld_PCoffset9_too_big),
        cmocka_unit_test(test_ld_PCoffset9_too_small),             
        cmocka_unit_test(test_ld_with_label),        
        cmocka_unit_test(test_ld_non_existent_label)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
