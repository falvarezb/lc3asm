#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_not_register(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "NOT R4,R5";
    uint16_t machine_instr;
    parse_not(asm_instr,&machine_instr);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 127);
    assert_int_equal(bytes[1], 153);
}

void test_not_wrong_register_DR(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "NOT R8,R1";
    uint16_t machine_instr;
    exit_t result = parse_not(asm_instr,&machine_instr);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "expected register but found R8");
}

void test_not_wrong_register_SR(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "NOT R0,SR1";
    uint16_t machine_instr;
    exit_t result = parse_not(asm_instr,&machine_instr);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "expected register but found SR1");
}

void test_not_wrong_element_in_instruction(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "NOT R0,R1,R2,R3";
    uint16_t machine_instr;
    exit_t result = parse_not(asm_instr, &machine_instr);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "unexpected token in NOT instruction");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_not_register),
        cmocka_unit_test(test_not_wrong_register_DR),
        cmocka_unit_test(test_not_wrong_register_SR),        
        cmocka_unit_test(test_not_wrong_element_in_instruction)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
