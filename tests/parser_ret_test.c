#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"
#include "lc3test.h"

void test_ret_right_instruction(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "RET";
    uint16_t machine_instr = parse_ret(asm_instr);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 192);
    assert_int_equal(bytes[1], 193);
}

void test_ret_wrong_instruction(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "ADD";
    uint16_t machine_instr = parse_ret(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected RET but found ADD\n");
}

void test_ret_wrong_element_in_instruction(void  __attribute__ ((unused)) **state) {
    char asm_instr[] = "RET R0,R1,R2,R3";
    uint16_t machine_instr = parse_ret(asm_instr);

    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "unexpected token in RET instruction\n");
}

// int main(int argc, char const *argv[]) {
//     const struct CMUnitTest tests[] = {
//         cmocka_unit_test(test_ret_right_instruction),
//         cmocka_unit_test(test_ret_wrong_instruction),
//         cmocka_unit_test(test_ret_wrong_element_in_instruction)
//     };
//     return cmocka_run_group_tests(tests, NULL, NULL);
// }
