
#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

static int setup(void **state) {
    clearerrdesc();
    initialize();
    linemetadata_t **tokenized_lines = malloc(ADDRESS_SPACE_CARDINALITY * sizeof(linemetadata_t *));
    for(size_t i = 0; i < ADDRESS_SPACE_CARDINALITY; i++) {
        //setting sentinel values
        tokenized_lines[i] = NULL;
    }
    *state = tokenized_lines;
    return 0;
}

static int teardown(void **state) {
    initialize();
    linemetadata_t **tokenized_lines = *state;
    free_tokenized_lines(tokenized_lines);
    return 0;
}

static void test_parse_fill_success(void  __attribute__((unused)) **state) {
    memaddr_t address_origin = 1;
    char *tokens[] = { ".FILL", "10" };
    linemetadata_t line_metadata = { .tokens = tokens, .num_tokens = 2 };
    exit_t result = parse_fill(&line_metadata, address_origin);
    if(result.code) {
        printf("\n\n==========================================\n");
        printf("%s\n", result.desc);
        printf("==========================================\n\n");
    }
    assert_int_equal(result.code, 0);

    unsigned char *bytes = (unsigned char *)&line_metadata.machine_instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 10);
    assert_int_equal(bytes[1], 0);
}

static void test_parse_fill_immediate_too_big(void  __attribute__((unused)) **state) {
    memaddr_t address_origin = 1;
    char *tokens[] = { ".FILL", "#70000" };
    linemetadata_t line_metadata = { .tokens = tokens, .num_tokens = 2, .line_number = 1 };
    exit_t result = parse_fill(&line_metadata, address_origin);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Immediate operand (#70000) out of range (-32768 to 65535)");
    free(result.desc);
}

static void test_parse_fill_immediate_too_small(void  __attribute__((unused)) **state) {
    memaddr_t address_origin = 1;
    char *tokens[] = { ".FILL", "#-33000" };
    linemetadata_t line_metadata = { .tokens = tokens, .num_tokens = 2, .line_number = 1 };
    exit_t result = parse_fill(&line_metadata, address_origin);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Immediate operand (#-33000) out of range (-32768 to 65535)");
    free(result.desc);
}

static void test_parse_stringz_with_escape_sequences(void  __attribute__((unused)) **state) {
    memaddr_t instruction_offset = 0;
    linemetadata_t **tokenized_lines = *state;
    char str[] = {'"', 'a', '\\', 'n', '"', '\0'};   
    char *tokens[] = { ".STRINGZ", str };
    linemetadata_t line_metadata = {.line = ".STRINGZ \"a\\n\"", .tokens = tokens, .num_tokens = 2, .line_number = 1 };
    exit_t result = parse_stringz(&line_metadata, tokenized_lines, &instruction_offset);
    assert_int_equal(result.code, 0);

    size_t idx;
    //1st instruction
    idx = 0;
    assert_null(tokenized_lines[idx]->tokens);
    assert_null(tokenized_lines[idx]->line);
    assert_int_equal(97, tokenized_lines[idx]->machine_instruction);

    //2nd instruction
    idx = 1;
    assert_null(tokenized_lines[idx]->tokens);
    assert_null(tokenized_lines[idx]->line);
    assert_int_equal(10, tokenized_lines[idx]->machine_instruction);

    //3rd instruction
    idx = 2;
    assert_null(tokenized_lines[idx]->tokens);
    assert_null(tokenized_lines[idx]->line);
    assert_int_equal(0, tokenized_lines[idx]->machine_instruction);

    //there is no more instructions
    idx = 3;
    assert_null(tokenized_lines[idx]);
}

static void test_parse_stringz_char_outside_quotation_marks(void  __attribute__((unused)) **state) {
    memaddr_t instruction_offset = 1;
    linemetadata_t **tokenized_lines = *state;
    char *tokens[] = { ".STRINGZ", "  a \"string content\"" };
    linemetadata_t line_metadata = {.tokens = tokens, .num_tokens = 2, .line_number = 1 };
    exit_t result = parse_stringz(&line_metadata, tokenized_lines, &instruction_offset);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Bad string ('  a \"string content\"')");
    free(result.desc);
}

static void test_parse_stringz_missing_quotation_marks(void  __attribute__((unused)) **state) {
    memaddr_t instruction_offset = 1;
    linemetadata_t **tokenized_lines = *state;
    char str[] = {'"', 'h', '\0'};   
    char *tokens[] = { ".STRINGZ", str };
    linemetadata_t line_metadata = {.line = ".STRINGZ  a \"h", .tokens = tokens, .num_tokens = 2, .line_number = 1 };
    exit_t result = parse_stringz(&line_metadata, tokenized_lines, &instruction_offset);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 1): Bad string ('.STRINGZ  a \"h')");
    free(result.desc);
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_parse_fill_success, setup, teardown),
        cmocka_unit_test_setup_teardown(test_parse_fill_immediate_too_big, setup, teardown),
        cmocka_unit_test_setup_teardown(test_parse_fill_immediate_too_small, setup, teardown),
        cmocka_unit_test_setup_teardown(test_parse_stringz_char_outside_quotation_marks, setup, teardown),
        cmocka_unit_test_setup_teardown(test_parse_stringz_missing_quotation_marks, setup, teardown),
        cmocka_unit_test_setup_teardown(test_parse_stringz_with_escape_sequences, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
