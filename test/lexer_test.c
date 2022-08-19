#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"
#include "../include/dict.h"

#define NUM_LINES 20

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

static void run_lexer_test(char *filename, linemetadata_t *tokenized_lines[]) {
    FILE *asm_file = fopen(filename, "r");
    if(!asm_file) {
        printf("error %d while reading file", errno);
        assert(false);
    }
    do_lexical_analysis(asm_file, tokenized_lines);
    fclose(asm_file);
}

static void assert_symbol_table(const char *label, size_t num_instruction) {
    node_t *node = lookup(label);
    assert_non_null(node);
    assert_int_equal(node->val, num_instruction);
}

///////////////////////////////////////////////////

static void test_lexer_without_labels_t1(void  __attribute__((unused)) **state) {
    linemetadata_t **tokenized_lines = *state;
    run_lexer_test("./test/testfiles/t1.asm", tokenized_lines);

    assert_null(next(true));

    size_t idx;
    //1st line
    idx = 0;
    assert_int_equal(2, tokenized_lines[idx]->num_tokens);
    assert_string_equal(".ORIG", tokenized_lines[idx]->tokens[0]);
    assert_string_equal("x3000", tokenized_lines[idx]->tokens[1]);
    assert_false(tokenized_lines[idx]->is_label_line);
    assert_int_equal(2, tokenized_lines[idx]->line_number);
    assert_int_equal(0, tokenized_lines[idx]->instruction_location);

    //2nd line
    idx = 1;
    assert_int_equal(6, tokenized_lines[idx]->num_tokens);
    assert_string_equal("ADD", tokenized_lines[idx]->tokens[0]);
    assert_string_equal("R0", tokenized_lines[idx]->tokens[1]);
    assert_string_equal("R0", tokenized_lines[idx]->tokens[2]);
    assert_string_equal("#1", tokenized_lines[idx]->tokens[3]);
    assert_string_equal(";", tokenized_lines[idx]->tokens[4]);
    assert_string_equal("comment", tokenized_lines[idx]->tokens[5]);
    assert_false(tokenized_lines[idx]->is_label_line);
    assert_int_equal(3, tokenized_lines[idx]->line_number);
    assert_int_equal(1, tokenized_lines[idx]->instruction_location);

    //3rd line
    idx = 2;
    assert_int_equal(1, tokenized_lines[idx]->num_tokens);
    assert_string_equal("HALT", tokenized_lines[idx]->tokens[0]);
    assert_false(tokenized_lines[idx]->is_label_line);
    assert_int_equal(4, tokenized_lines[idx]->line_number);
    assert_int_equal(2, tokenized_lines[idx]->instruction_location);

    // free_line_metadata(tokenized_lines);
}

void test_lexer_t2(void  __attribute__((unused)) **state) {
    linemetadata_t **tokenized_lines = *state;
    run_lexer_test("./test/testfiles/t2.asm", tokenized_lines);

    assert_symbol_table("LABEL", 4);

    size_t idx;
    //1st line
    idx = 0;
    assert_int_equal(2, tokenized_lines[idx]->num_tokens);
    assert_string_equal(".ORIG", tokenized_lines[idx]->tokens[0]);
    assert_string_equal("x3000", tokenized_lines[idx]->tokens[1]);
    assert_false(tokenized_lines[idx]->is_label_line);
    assert_int_equal(4, tokenized_lines[idx]->line_number);
    assert_int_equal(0, tokenized_lines[idx]->instruction_location);

    //2nd line
    idx = 1;
    assert_int_equal(2, tokenized_lines[idx]->num_tokens);
    assert_string_equal("JSR", tokenized_lines[idx]->tokens[0]);
    assert_string_equal("LABEL", tokenized_lines[idx]->tokens[1]);
    assert_false(tokenized_lines[idx]->is_label_line);
    assert_int_equal(5, tokenized_lines[idx]->line_number);
    assert_int_equal(1, tokenized_lines[idx]->instruction_location);

    //3rd line
    idx = 2;
    assert_int_equal(4, tokenized_lines[idx]->num_tokens);
    assert_string_equal("ADD", tokenized_lines[idx]->tokens[0]);
    assert_string_equal("R0", tokenized_lines[idx]->tokens[1]);
    assert_string_equal("R0", tokenized_lines[idx]->tokens[2]);
    assert_string_equal("#1", tokenized_lines[idx]->tokens[3]);
    assert_false(tokenized_lines[idx]->is_label_line);
    assert_int_equal(6, tokenized_lines[idx]->line_number);
    assert_int_equal(2, tokenized_lines[idx]->instruction_location);

    //4th line
    idx = 3;
    assert_int_equal(1, tokenized_lines[idx]->num_tokens);
    assert_string_equal("HALT", tokenized_lines[idx]->tokens[0]);
    assert_false(tokenized_lines[idx]->is_label_line);
    assert_int_equal(7, tokenized_lines[idx]->line_number);
    assert_int_equal(3, tokenized_lines[idx]->instruction_location);

    //5th line
    idx = 4;
    assert_int_equal(6, tokenized_lines[idx]->num_tokens);
    assert_string_equal("ADD", tokenized_lines[idx]->tokens[0]);
    assert_string_equal("R0", tokenized_lines[idx]->tokens[1]);
    assert_string_equal("R1", tokenized_lines[idx]->tokens[2]);
    assert_string_equal("R2", tokenized_lines[idx]->tokens[3]);
    assert_string_equal(";", tokenized_lines[idx]->tokens[4]);
    assert_string_equal("comment", tokenized_lines[idx]->tokens[5]);
    assert_false(tokenized_lines[idx]->is_label_line);
    assert_int_equal(11, tokenized_lines[idx]->line_number);
    assert_int_equal(4, tokenized_lines[idx]->instruction_location);    
}

static void test_lexer_t3(void  __attribute__((unused)) **state) {
    linemetadata_t **tokenized_lines = *state;
    run_lexer_test("./test/testfiles/t3.asm", tokenized_lines);

    assert_symbol_table("LABEL", 4);

    size_t idx;
    //5th line
    idx = 4;
    assert_int_equal(4, tokenized_lines[idx]->num_tokens);
    assert_string_equal("ADD", tokenized_lines[idx]->tokens[0]);
    assert_string_equal("R0", tokenized_lines[idx]->tokens[1]);
    assert_string_equal("R1", tokenized_lines[idx]->tokens[2]);
    assert_string_equal("R2", tokenized_lines[idx]->tokens[3]);
    assert_true(tokenized_lines[idx]->is_label_line);
    assert_int_equal(10, tokenized_lines[idx]->line_number);
    assert_int_equal(4, tokenized_lines[idx]->instruction_location);    
}

static void test_lexer_t4(void  __attribute__((unused)) **state) {
    linemetadata_t **tokenized_lines = *state;
    run_lexer_test("./test/testfiles/t4.asm", tokenized_lines);

    assert_symbol_table("LABEL1", 4);
    assert_symbol_table("LABEL2", 2);
    assert_symbol_table("LABEL3", 3);
    assert_symbol_table("LABEL4", 5);
    assert_symbol_table("LABEL5", 4);

    //free_line_metadata(tokenized_lines);
}

static void test_lexer_t5(void  __attribute__((unused)) **state) {
    linemetadata_t **tokenized_lines = *state;
    run_lexer_test("./test/testfiles/t5.asm", tokenized_lines);

    assert_symbol_table("LABEL1", 4);

    size_t idx;
    //5th line
    idx = 4;
    assert_int_equal(5, tokenized_lines[idx]->num_tokens);
    assert_string_equal("LABEL2", tokenized_lines[idx]->tokens[0]);
    assert_string_equal("ADD", tokenized_lines[idx]->tokens[1]);
    assert_string_equal("R0", tokenized_lines[idx]->tokens[2]);
    assert_string_equal("R1", tokenized_lines[idx]->tokens[3]);
    assert_string_equal("R2", tokenized_lines[idx]->tokens[4]);
    assert_true(tokenized_lines[idx]->is_label_line);
    assert_int_equal(10, tokenized_lines[idx]->line_number);
    assert_int_equal(4, tokenized_lines[idx]->instruction_location);    
}


int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_lexer_without_labels_t1, setup, teardown),
        cmocka_unit_test_setup_teardown(test_lexer_t2, setup, teardown),
        cmocka_unit_test_setup_teardown(test_lexer_t3, setup, teardown),
        cmocka_unit_test_setup_teardown(test_lexer_t4, setup, teardown),
        cmocka_unit_test_setup_teardown(test_lexer_t5, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
