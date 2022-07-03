#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"
#include "../include/dict.h"

int run_sym_test(const char *asm_file_name) {
    initialize();
    FILE *source_file = fopen(asm_file_name, "r");

    int result = compute_symbol_table(source_file);
    fclose(source_file);
    return result;
}

void assert_sym(const char *label, size_t num_instruction) {
    node_t *node = lookup(label);
    assert_non_null(node);
    assert_int_equal(node->val, num_instruction);
    delete(label);
}

void run_file_creation_test(char* asm_file_name, char* obj_file_name, char* actual_obj_file_name) {
    FILE *source_file = fopen(asm_file_name, "r");
    FILE *actual_obj_file = fopen(actual_obj_file_name, "w");

    second_pass_parse(source_file, actual_obj_file);
    fclose(source_file);
    fclose(actual_obj_file);

    FILE *expected_obj_file = fopen(obj_file_name, "r");
    actual_obj_file = fopen(actual_obj_file_name, "r");

    char buf_expected[2];
    char buf_actual[2];
    size_t num_lines = 1;

    size_t read;
    while((read = fread(buf_expected, 1, 2, expected_obj_file)) == 2) {
        read = fread(buf_actual, 1, 2, actual_obj_file);
        printf("line checked: %zu\n", num_lines);
        assert_int_equal(read, 2);
        assert_true(buf_expected[0] == buf_actual[0] && buf_expected[1] == buf_actual[1]);
        num_lines++;
    }

    assert_int_equal(read, fread(buf_actual, 1, 2, actual_obj_file));
    fclose(expected_obj_file);
    fclose(actual_obj_file);
}

void test_object_file_creation_t1(void  __attribute__((unused)) **state) {
    run_file_creation_test("./test/t1.asm", "./test/t1.obj", "./test/t1.actual.obj");
}

void test_symbol_table_calculation_t2(void  __attribute__((unused)) **state) {
    run_sym_test("./test/t2.asm");
    assert_sym("LABEL", 0x3003);
}

void test_symbol_table_calculation_t3(void  __attribute__((unused)) **state) {
    run_sym_test("./test/t3.asm");
    assert_sym("LABEL", 0x3003);
}

void test_symbol_table_calculation_t4(void  __attribute__((unused)) **state) {
    run_sym_test("./test/t4.asm");
    assert_sym("LABEL1", 0x3003);
    assert_sym("LABEL2", 0x3001);
    assert_sym("LABEL3", 0x3002);
    assert_sym("LABEL4", 0x3004);
    assert_sym("LABEL5", 0x3003);
}

void test_symbol_table_calculation_t5(void  __attribute__((unused)) **state) {
    assert_int_equal(run_sym_test("./test/t5.asm"), EXIT_FAILURE);
    assert_string_equal(errdesc, "invalid opcode ('LABEL2')");
}

void test_symbol_table_serialization(void  __attribute__((unused)) **state) {
    initialize();
    add("LABEL", 0x3003);
    FILE *actual_sym_file = fopen("./test/t2.actual.sym", "w");
    serialize_symbol_table(actual_sym_file);
    fclose(actual_sym_file);

    //test symbol table serialization
    FILE *expected_sym_file = fopen("./test/t2.sym", "r");
    actual_sym_file = fopen("./test/t2.actual.sym", "r");

    size_t num_lines = 1;
    char *line_expected = NULL;
    char *line_actual = NULL;
    size_t len = 0;
    ssize_t read_expected;
    while((read_expected = getline(&line_expected, &len, expected_sym_file)) != -1) {
        printf("line checked: %zu\n", num_lines);
        ssize_t read_actual;
        if((read_actual = getline(&line_actual, &len, actual_sym_file)) != -1) {
            assert_true(strcmp(line_expected, line_actual) == 0);
            num_lines++;
        }
        else {
            //this will fail as read_actual == 1 != read_expected
            assert_int_equal(read_expected, read_actual);
        }
    }
    fclose(expected_sym_file);
    fclose(actual_sym_file);
    free(line_expected);
    free(line_actual);
    node_t *label = lookup("LABEL");
    delete(label->key);
}

void test_symbol_table_serialization_failure(void  __attribute__((unused)) **state) {
    initialize();
    node_t *label = add("LABEL", 0x3003);
    FILE *actual_sym_file = fopen("./test/t2.actual.sym", "r");
    int result = serialize_symbol_table(actual_sym_file);
    assert_string_equal(errdesc, "error when writing serialized symbol table to file");
    
    assert_int_equal(result, 1);

    fclose(actual_sym_file);
    delete(label->key);
}


int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_object_file_creation_t1),
        cmocka_unit_test(test_symbol_table_calculation_t2),
        cmocka_unit_test(test_symbol_table_calculation_t3),
        cmocka_unit_test(test_symbol_table_calculation_t4),
        cmocka_unit_test(test_symbol_table_calculation_t5),
        cmocka_unit_test(test_symbol_table_serialization),
        cmocka_unit_test(test_symbol_table_serialization_failure)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
