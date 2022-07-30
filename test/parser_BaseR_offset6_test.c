#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

static int setup(void **state) {
    clearerrdesc();
    initialize();
    return 0;
}

static int teardown(void **state) {
    initialize();
    return 0;
}

static void test_ldr_right_instr(void __attribute__ ((unused)) **state) {  
    char *tokens[] = {"DOES NOT MATTER", "R0","R1","1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4};      
    parse_base_plus_offset_addressing_mode(&line_metadata,LDR);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 65);
    assert_int_equal(bytes[1], 96);
}

static void test_str_right_instr(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0", "R1","1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4};      
    parse_base_plus_offset_addressing_mode(&line_metadata,STR);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 65);
    assert_int_equal(bytes[1], 112);
}

static void test_ldr_wrong_DR(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R9","R0","3"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 1};      
    exit_t result = parse_base_plus_offset_addressing_mode(&line_metadata,LDR);
    
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Expected register but found R9");
}

static void test_ldr_wrong_BaseR(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0","R8","3"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 1};      
    exit_t result = parse_base_plus_offset_addressing_mode(&line_metadata,LDR);
    
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Expected register but found R8");
}

static void test_ldr_offset6_too_big(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0","R1","40"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 1};      
    exit_t result = parse_base_plus_offset_addressing_mode(&line_metadata,LDR);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of offset 40 is outside the range [-32, 31]");
}

static void test_ldr_offset6_too_small(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0","R1","-40"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 1};      
    exit_t result = parse_base_plus_offset_addressing_mode(&line_metadata,LDR);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of offset -40 is outside the range [-32, 31]");
}


static void test_ldr_with_label(void __attribute__ ((unused))  **state) {    
    add("LABEL", 3);    
    char *tokens[] = {"DOES NOT MATTER", "R0","R1","LABEL"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .instruction_location = 1};      
    exit_t result = parse_base_plus_offset_addressing_mode(&line_metadata,LDR);   
    assert_int_equal(result.code, 0);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 65);
    assert_int_equal(bytes[1], 96);
    initialize();
}

static void test_ldr_non_existent_label(void __attribute__ ((unused))  **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0","R1","NON_EXISTENT_LABEL"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 4, .line_number = 1};      
    exit_t result = parse_base_plus_offset_addressing_mode(&line_metadata,LD); 
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Symbol not found ('NON_EXISTENT_LABEL')");
}

int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused)) *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_ldr_right_instr, setup, teardown),
        cmocka_unit_test_setup_teardown(test_str_right_instr, setup, teardown),
        cmocka_unit_test_setup_teardown(test_ldr_wrong_DR, setup, teardown),  
        cmocka_unit_test_setup_teardown(test_ldr_wrong_BaseR, setup, teardown),    
        cmocka_unit_test_setup_teardown(test_ldr_offset6_too_big, setup, teardown),
        cmocka_unit_test_setup_teardown(test_ldr_offset6_too_small, setup, teardown),             
        cmocka_unit_test_setup_teardown(test_ldr_with_label, setup, teardown),        
        cmocka_unit_test_setup_teardown(test_ldr_non_existent_label, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
