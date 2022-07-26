#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_add_SR2(void  __attribute__((unused)) **state) {        
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "R2"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4};
    parse_binary_operation(&line_metadata, ADD);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 66);
    assert_int_equal(bytes[1], 16);
}

void test_and_SR2(void  __attribute__((unused)) **state) {        
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "R2"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4};
    parse_binary_operation(&line_metadata, AND);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 66);
    assert_int_equal(bytes[1], 80);
}

void test_add_imm5_decimal(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "#13"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4};
    parse_binary_operation(&line_metadata, ADD);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 109);
    assert_int_equal(bytes[1], 16);
}

void test_add_imm5_hex(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "xa"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4};
    parse_binary_operation(&line_metadata, ADD);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 106);
    assert_int_equal(bytes[1], 16);
}

void test_add_wrong_register_DR(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R8", "R1", "#13"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 0};
    exit_t result = parse_binary_operation(&line_metadata, ADD);    
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Expected register but found R8");
    free(result.desc);
}

void test_add_wrong_register_SR1(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "SR1", "#13"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 0};
    exit_t result = parse_binary_operation(&line_metadata, ADD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Expected register but found SR1");
}

void test_add_wrong_imm5_too_big_dec(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "#16"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 0};
    exit_t result = parse_binary_operation(&line_metadata, ADD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (16) outside of range (-16 to 15)");
}

void test_add_wrong_imm5_too_small_dec(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "#-17"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 0};
    exit_t result = parse_binary_operation(&line_metadata, ADD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (-17) outside of range (-16 to 15)");
}

void test_add_wrong_imm5_too_big_hex(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "xf1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 0};
    exit_t result = parse_binary_operation(&line_metadata, ADD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (f1) outside of range (-16 to 15)");
}

void test_add_wrong_imm5_too_small_hex(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "x-f2"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 0};
    exit_t result = parse_binary_operation(&line_metadata, ADD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate operand (-f2) outside of range (-16 to 15)");
}

void test_add_wrong_imm5_format(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "0"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 0};
    exit_t result = parse_binary_operation(&line_metadata, ADD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate 0 must be decimal or hex");
}

void test_add_wrong_imm5_number(void  __attribute__((unused)) **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1", "#y"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 0};
    exit_t result = parse_binary_operation(&line_metadata, ADD);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Immediate #y is not a numeric value");
}


int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add_SR2),
        cmocka_unit_test(test_and_SR2),
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
