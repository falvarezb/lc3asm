#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"
#include "../include/dict.h"

static int setup(void **state) {
    clearerrdesc();
    initialize();
    return 0;
}

static int teardown(void **state) {
    initialize();
    return 0;
}

static int run_symbol_table_test(const char *asm_file_name) {        
    int result = compute_symbol_table(asm_file_name);    
    return result;
}

static void assert_symbol_table(const char *label, size_t num_instruction) {
    node_t *node = lookup(label);
    assert_non_null(node);
    assert_int_equal(node->val, num_instruction);
    delete(label);
}

static void run_second_pass_test(char* asm_file_name, char* expected_obj_file_name, char* actual_obj_file_name) {
    second_pass_parse(asm_file_name, actual_obj_file_name);    

    FILE *expected_obj_file = fopen(expected_obj_file_name, "r");
    FILE *actual_obj_file = fopen(actual_obj_file_name, "r");

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

static void run_assemble_test(char* asm_file_name, char* expected_obj_file_name, char* actual_obj_file_name) {    
    assemble(asm_file_name);
    printf("\nmyerror:%s\n", errdesc);

    FILE *expected_obj_file = fopen(expected_obj_file_name, "r");
    FILE *actual_obj_file = fopen(actual_obj_file_name, "r");

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

///////////////////////////////////////////////////

static void test_second_pass_without_labels_t1(void  __attribute__((unused)) **state) {
    run_second_pass_test("./test/t1.asm", "./test/t1.expected.obj", "./test/t1.obj");
}

static void test_second_pass_with_labels_t2(void  __attribute__((unused)) **state) {    
    add("LABEL", 0x3003);
    run_second_pass_test("./test/t2.asm", "./test/t2.expected.obj", "./test/t2.obj");    
}

void test_symbol_table_t2(void  __attribute__((unused)) **state) {
    run_symbol_table_test("./test/t2.asm");
    assert_symbol_table("LABEL", 0x3003);
}

static void test_symbol_table_t3(void  __attribute__((unused)) **state) {
    run_symbol_table_test("./test/t3.asm");
    assert_symbol_table("LABEL", 0x3003);
}

static void test_symbol_table_t4(void  __attribute__((unused)) **state) {
    run_symbol_table_test("./test/t4.asm");
    assert_symbol_table("LABEL1", 0x3003);
    assert_symbol_table("LABEL2", 0x3001);
    assert_symbol_table("LABEL3", 0x3002);
    assert_symbol_table("LABEL4", 0x3004);
    assert_symbol_table("LABEL5", 0x3003);
}

static void test_symbol_table_t5(void  __attribute__((unused)) **state) {
    assert_int_equal(run_symbol_table_test("./test/t5.asm"), EXIT_FAILURE);
    assert_string_equal(errdesc, "invalid opcode ('LABEL2')");
}

static void test_symbol_table_serialization(void  __attribute__((unused)) **state) {    
    add("LABEL", 0x3003);
    const char *actual_sym_file_name = "./test/t2.actual.sym";
    serialize_symbol_table(actual_sym_file_name);    

    //test symbol table serialization
    FILE *expected_sym_file = fopen("./test/t2.sym", "r");
    FILE *actual_sym_file = fopen("./test/t2.actual.sym", "r");

    size_t num_lines = 1;
    char *line_expected = NULL;
    char *line_actual = NULL;
    size_t len_expected = 0;
    size_t len_actual = 0;
    ssize_t read_expected;
    while((read_expected = getline(&line_expected, &len_expected, expected_sym_file)) != -1) {
        printf("line checked: %zu\n", num_lines);
        ssize_t read_actual;
        if((read_actual = getline(&line_actual, &len_actual, actual_sym_file)) != -1) {
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
}

static void test_symbol_table_serialization_failure(void  __attribute__((unused)) **state) {    
    add("LABEL", 0x3003);
    char *actual_sym_file_name = "./test/test/t2.actual.sym";
    int result = serialize_symbol_table(actual_sym_file_name);
    assert_string_equal(errdesc, "error when writing serialized symbol table to file");    
    assert_int_equal(result, 1);        
}

static void test_assemble_without_labels_t1(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/t1.asm", "./test/t1.expected.obj", "./test/t1.obj");    
}

static void test_assemble_with_labels_t2(void  __attribute__((unused)) **state) {    
    run_assemble_test("./test/t2.asm", "./test/t2.expected.obj", "./test/t2.obj");
}

static void test_first_pass_wrong_orig_address_t6(void  __attribute__((unused)) **state) {    
    int result = first_pass_parse("./test/t6.asm", "does not matter");
    assert_int_equal(result, 1);    
    assert_string_equal(errdesc, "immediate operand (545677767) outside of range (0 to 65535)");
}

static void test_first_pass_missing_orig_t7(void  __attribute__((unused)) **state) {    
    int result = first_pass_parse("./test/t7.asm", "does not matter");
    assert_int_equal(result, 1);
    assert_string_equal(errdesc, "ERROR (line 4): Instruction not preceeded by a .orig directive");
}

static void test_first_pass_missing_orig_address_t8(void  __attribute__((unused)) **state) {    
    int result = first_pass_parse("./test/t8.asm", "does not matter");
    assert_int_equal(result, 1);
    assert_string_equal(errdesc, "ERROR (line 4): Immediate expected");
}

static void test_missing_assembly_file(void  __attribute__((unused)) **state) {    
    int result = assemble("./test/test/random.asm");
    assert_int_equal(result, 1);
    assert_string_equal(errdesc, "ERROR: Couldn't read file (./test/test/random.asm)");
}

static void test_wrong_assembly_file_extension(void  __attribute__((unused)) **state) {    
    int result = assemble("./test/t2.copy");
    assert_int_equal(result, 1);
    assert_string_equal(errdesc, "ERROR: Input file must have .asm suffix ('./test/t2.copy')");
}


int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_second_pass_without_labels_t1, setup, teardown),
        cmocka_unit_test_setup_teardown(test_second_pass_with_labels_t2, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_t2, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_t3, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_t4, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_t5, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_serialization, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_serialization_failure, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_without_labels_t1, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_with_labels_t2, setup, teardown),
        cmocka_unit_test_setup_teardown(test_first_pass_wrong_orig_address_t6, setup, teardown),
        cmocka_unit_test_setup_teardown(test_first_pass_missing_orig_t7, setup, teardown),
        cmocka_unit_test_setup_teardown(test_first_pass_missing_orig_address_t8, setup, teardown),
        cmocka_unit_test_setup_teardown(test_missing_assembly_file, setup, teardown),
        cmocka_unit_test_setup_teardown(test_wrong_assembly_file_extension, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
