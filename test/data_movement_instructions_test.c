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

void test_ld_right_instr(void __attribute__ ((unused)) **state) {  
    char *tokens[] = {"DOES NOT MATTER", "R0","1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3};      
    parse_pc_relative_addressing_mode(&line_metadata,LD);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 32);
}

void test_st_right_instr(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0","1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3};      
    parse_pc_relative_addressing_mode(&line_metadata,ST);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 48);
}

void test_ldi_right_instr(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0","1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3};      
    parse_pc_relative_addressing_mode(&line_metadata,LDI);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 160);
}

void test_sti_right_instr(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0","1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3};      
    parse_pc_relative_addressing_mode(&line_metadata,STI);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 176);
}

void test_lea_right_instr(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0","1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3};      
    parse_pc_relative_addressing_mode(&line_metadata,LEA);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 224);
}

void test_ld_PCoffset9_wrong_register(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R9","3"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3, .line_number = 1};      
    exit_t result = parse_pc_relative_addressing_mode(&line_metadata,LD);
    
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Expected register but found R9");
}

void test_ld_PCoffset9_too_big(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0","300"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3, .line_number = 1};      
    exit_t result = parse_pc_relative_addressing_mode(&line_metadata,LD);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of offset 300 is outside the range [-256, 255]");
}

void test_ld_PCoffset9_too_small(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "R0","-300"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3, .line_number = 1};      
    exit_t result = parse_pc_relative_addressing_mode(&line_metadata,LD);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of offset -300 is outside the range [-256, 255]");
}


void test_ld_with_label(void __attribute__ ((unused))  **state) {    
    add("LABEL", 3);    
    char *tokens[] = {"DOES NOT MATTER", "R0","LABEL"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3, .instruction_location = 1};      
    exit_t result = parse_pc_relative_addressing_mode(&line_metadata,LD);   
    assert_int_equal(result.code, 0);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 32);
    initialize();
}

void test_ld_non_existent_label(void __attribute__ ((unused))  **state) {
    char *tokens[] = {"DOES NOT MATTER", "R0","NON_EXISTENT_LABEL"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 3, .line_number = 1};      
    exit_t result = parse_pc_relative_addressing_mode(&line_metadata,LD); 
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Symbol not found ('NON_EXISTENT_LABEL')");
}

int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused)) *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_ld_right_instr, setup, teardown),
        cmocka_unit_test_setup_teardown(test_st_right_instr, setup, teardown),
        cmocka_unit_test_setup_teardown(test_ldi_right_instr, setup, teardown),
        cmocka_unit_test_setup_teardown(test_sti_right_instr, setup, teardown),
        cmocka_unit_test_setup_teardown(test_lea_right_instr, setup, teardown),
        cmocka_unit_test_setup_teardown(test_ld_PCoffset9_wrong_register, setup, teardown),    
        cmocka_unit_test_setup_teardown(test_ld_PCoffset9_too_big, setup, teardown),
        cmocka_unit_test_setup_teardown(test_ld_PCoffset9_too_small, setup, teardown),             
        cmocka_unit_test_setup_teardown(test_ld_with_label, setup, teardown),        
        cmocka_unit_test_setup_teardown(test_ld_non_existent_label, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
