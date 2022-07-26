#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

static void test_br(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_br(&line_metadata, 7);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 14);
}

static void test_brp(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_br(&line_metadata, 1);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 2);
}

static void test_brz(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_br(&line_metadata, 2);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 4);
}

static void test_brn(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_br(&line_metadata, 4);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 8);
}

static void test_brzp(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_br(&line_metadata, 3);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 6);
}

static void test_brnp(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_br(&line_metadata, 5);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 10);
}

static void test_brnz(void __attribute__ ((unused)) **state) {    
    char *tokens[] = {"DOES NOT MATTER", "1"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2};
    parse_br(&line_metadata, 6);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 12);
}

static void test_br_PCoffset9_too_big(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "300"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1};
    exit_t result = parse_br(&line_metadata, 7);
    
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of offset 300 is outside the range [-256, 255]");
}

static void test_br_PCoffset9_too_small(void __attribute__ ((unused))  **state) {    
    char *tokens[] = {"DOES NOT MATTER", "-300"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1};
    exit_t result = parse_br(&line_metadata, 7);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Value of offset -300 is outside the range [-256, 255]");
}


static void test_br_with_label(void __attribute__ ((unused))  **state) {
    initialize();
    add("LABEL", 3); 

    char *tokens[] = {"DOES NOT MATTER", "LABEL"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .instruction_location = 1};
    parse_br(&line_metadata, 7);
    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;

    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 14);
    initialize();
}

static void test_br_non_existent_label(void __attribute__ ((unused))  **state) {
    initialize();    
    char *tokens[] = {"DOES NOT MATTER", "NON_EXISTENT_LABEL"};
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1};
    exit_t result = parse_br(&line_metadata, 7);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Symbol not found ('NON_EXISTENT_LABEL')");
}

int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused)) *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_br),
        cmocka_unit_test(test_brp),
        cmocka_unit_test(test_brz),
        cmocka_unit_test(test_brn),
        cmocka_unit_test(test_brzp),
        cmocka_unit_test(test_brnp),
        cmocka_unit_test(test_brnz),
        cmocka_unit_test(test_br_PCoffset9_too_big),
        cmocka_unit_test(test_br_PCoffset9_too_small),             
        cmocka_unit_test(test_br_with_label),
        cmocka_unit_test(test_br_non_existent_label)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
