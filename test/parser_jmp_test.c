#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_jmp_register(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "JMP R6";
    uint16_t machine_instr;
    parse_jmp(asm_instr,&machine_instr);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 128);
    assert_int_equal(bytes[1], 193);
}

void test_jmp_wrong_register_BaseR(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "JMP R8";
    uint16_t machine_instr;
    exit_t result = parse_jmp(asm_instr,&machine_instr);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "expected register but found R8");
}

void test_jmp_wrong_element_in_instruction(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "JMP R0,R1";
    uint16_t machine_instr;
    exit_t result = parse_jmp(asm_instr,&machine_instr);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "unexpected token in JMP instruction");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_jmp_register),
        cmocka_unit_test(test_jmp_wrong_register_BaseR),                
        cmocka_unit_test(test_jmp_wrong_element_in_instruction)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
