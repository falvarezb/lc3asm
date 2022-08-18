
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

static void compare_chars(int ch, uint16_t instruction) {
    unsigned char *bytes = (unsigned char *)&instruction;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], ch);
    assert_int_equal(bytes[1], 0);
}

static void test_parse_stringz(void  __attribute__((unused)) **state) {
    linemetadata_t **tokenized_lines = *state;
    memaddr_t instruction_offset = 2;
    char *tokens[] = { ".STRINGZ", "hey\ny" };
    linemetadata_t line_metadata = { .tokens = tokens, .num_tokens = 2 };
    parse_stringz(&line_metadata, tokenized_lines, &instruction_offset);

    compare_chars('h', tokenized_lines[2]->machine_instruction);
    compare_chars('e', tokenized_lines[3]->machine_instruction);
    compare_chars('y', tokenized_lines[4]->machine_instruction);
    compare_chars('\n', tokenized_lines[5]->machine_instruction);
    compare_chars('y', tokenized_lines[6]->machine_instruction);
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_parse_stringz, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}