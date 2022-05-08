#ifndef FAB_LC3_TEST
#define FAB_LC3_TEST
#include "lc3test.h"


// ADD tests
void test_add_SR2(void **state);
void test_add_imm5_decimal(void **state);
void test_add_imm5_hex(void **state);
void test_add_wrong_register_DR(void **state);
void test_add_wrong_register_SR1(void **state);
void test_add_wrong_imm5_too_big_dec(void **state);
void test_add_wrong_imm5_too_small_dec(void **state);
void test_add_wrong_imm5_too_big_hex(void **state);
void test_add_wrong_imm5_too_small_hex(void **state);
void test_add_wrong_imm5_format(void **state);
void test_add_wrong_instruction(void **state);
void test_add_wrong_element_in_instruction(void **state);

// AND tests
void test_and_SR2(void **state);
void test_and_imm5_decimal(void **state);
void test_and_imm5_hex(void **state);
void test_and_wrong_register_DR(void **state);
void test_and_wrong_register_SR1(void **state);
void test_and_wrong_imm5_too_big_dec(void **state);
void test_and_wrong_imm5_too_small_dec(void **state);
void test_and_wrong_imm5_too_big_hex(void **state);
void test_and_wrong_imm5_too_small_hex(void **state);
void test_and_wrong_imm5_format(void **state);
void test_and_wrong_instruction(void **state);
void test_and_wrong_element_in_instruction(void **state);

// NOT tests
void test_not_register(void **state);
void test_not_wrong_register_DR(void **state);
void test_not_wrong_register_SR(void **state);
void test_not_wrong_instruction(void **state);
void test_not_wrong_element_in_instruction(void **state);

// RET tests
void test_ret_right_instruction(void **state);
void test_ret_wrong_instruction(void **state);
void test_ret_wrong_element_in_instruction(void **state);

// JMP tests
void test_jmp_register(void **state);
void test_jmp_wrong_register_BaseR(void **state);
void test_jmp_wrong_instruction(void **state);
void test_jmp_wrong_element_in_instruction(void **state);

#endif
