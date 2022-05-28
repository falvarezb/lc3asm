#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "lc3.h"
#include "lc3test.h"

int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused))  *argv[]) {
    const struct CMUnitTest tests[] = {
        // ADD tests
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
        cmocka_unit_test(test_add_wrong_imm5_number),
        cmocka_unit_test(test_add_wrong_instruction),
        cmocka_unit_test(test_add_wrong_element_in_instruction),  
        // AND tests
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
        cmocka_unit_test(test_and_wrong_instruction),
        cmocka_unit_test(test_and_wrong_element_in_instruction),  
        // NOT tests
        cmocka_unit_test(test_not_register),
        cmocka_unit_test(test_not_wrong_register_DR),
        cmocka_unit_test(test_not_wrong_register_SR),
        cmocka_unit_test(test_not_wrong_instruction),
        cmocka_unit_test(test_not_wrong_element_in_instruction),
        //RET tests
        cmocka_unit_test(test_ret_right_instruction),
        cmocka_unit_test(test_ret_wrong_instruction),
        cmocka_unit_test(test_ret_wrong_element_in_instruction),  
        // JMP tests
        cmocka_unit_test(test_jmp_register),
        cmocka_unit_test(test_jmp_wrong_register_BaseR),        
        cmocka_unit_test(test_jmp_wrong_instruction),
        cmocka_unit_test(test_jmp_wrong_element_in_instruction)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
