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

static void assert_symbol_table(const char *label, size_t num_instruction) {
    node_t *node = lookup(label);
    assert_non_null(node);
    assert_int_equal(node->val, num_instruction);
}

static void run_assemble_test(char *asm_file_name, char *expected_obj_file_name, char *actual_obj_file_name) {
    exit_t result = assemble(asm_file_name);
    if(result.code) {
        printf("\n\n==========================================\n");
        printf("%s\n", result.desc);
        printf("==========================================\n\n");
    }
    assert_int_equal(result.code, 0);

    FILE *expected_obj_file = fopen(expected_obj_file_name, "r");
    FILE *actual_obj_file = fopen(actual_obj_file_name, "r");

    char buf_expected[2];
    char buf_actual[2];
    size_t num_lines = 1;

    size_t read;
    while((read = fread(buf_expected, 1, 2, expected_obj_file)) == 2) {
        read = fread(buf_actual, 1, 2, actual_obj_file);
        // printf("line checked: %zu\n", num_lines);
        assert_int_equal(read, 2);
        assert_true(buf_expected[0] == buf_actual[0] && buf_expected[1] == buf_actual[1]);
        num_lines++;
    }

    assert_int_equal(read, fread(buf_actual, 1, 2, actual_obj_file));
    fclose(expected_obj_file);
    fclose(actual_obj_file);
}

static void test_symbol_table_t2(void  __attribute__((unused)) **state) {
    assemble("./test/testfiles/t2.asm");
    assert_symbol_table("LABEL", 0x3003);
}

static void test_symbol_table_t3(void  __attribute__((unused)) **state) {
    assemble("./test/testfiles/t3.asm");
    assert_symbol_table("LABEL", 0x3003);
}

static void test_symbol_table_t4(void  __attribute__((unused)) **state) {
    assemble("./test/testfiles/t4.asm");
    assert_symbol_table("LABEL1", 0x3003);
    assert_symbol_table("LABEL2", 0x3001);
    assert_symbol_table("LABEL3", 0x3002);
    assert_symbol_table("LABEL4", 0x3004);
    assert_symbol_table("LABEL5", 0x3003);
}

static void test_two_labels_same_line_t5(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/testfiles/t5.asm");
    assert_int_equal(result.code, EXIT_FAILURE);
    assert_string_equal(result.desc, "ERROR (line 10): Invalid opcode ('LABEL2')");
    free(result.desc);
}

static void test_assemble_without_labels_t1(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/t1.asm", "./test/testfiles/t1.expected.obj", "./test/testfiles/t1.obj");
}

static void test_assemble_with_labels_t2(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/t2.asm", "./test/testfiles/t2.expected.obj", "./test/testfiles/t2.obj");
}

static void test_assemble_wrong_orig_address_t6(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/testfiles/t6.asm");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 4): Immediate operand (545677767) outside of range (0 to 65535)");
    free(result.desc);
}

static void test_assemble_missing_orig_t7(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/testfiles/t7.asm");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 4): Instruction not preceeded by a .orig directive");
    free(result.desc);
}

static void test_assemble_missing_orig_address_t8(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/testfiles/t8.asm");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 4): Immediate expected");
    free(result.desc);
}

static void test_assemble_stringz_t9(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/t9.asm", "./test/testfiles/t9.expected.obj", "./test/testfiles/t9.obj");
}

static void test_assemble_blkw_t10(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/t10.asm", "./test/testfiles/t10.expected.obj", "./test/testfiles/t10.obj");
}

static void test_assemble_with_labels_t11(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/t11.asm", "./test/testfiles/t11.expected.obj", "./test/testfiles/t11.obj");
}

static void test_assemble_with_wrong_stringz_t12(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/testfiles/t12.asm");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 6): Bad string ('    \\  \"a\\n'\\\\\\t\\e\\\"b\"    \n')");
    free(result.desc);
}

static void test_missing_assembly_file(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/testfiles/test/testfiles/random.asm");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR: Couldn't read file (./test/testfiles/test/testfiles/random.asm)");
    free(result.desc);
}

static void test_wrong_assembly_file_extension(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/testfiles/t2.copy");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR: Input file must have .asm suffix ('./test/testfiles/t2.copy')");
    free(result.desc);
}

static void test_symbol_table_serialization(void  __attribute__((unused)) **state) {
    add("LABEL", 4);
    FILE *actual_sym_file = fopen("./test/testfiles/t2.sym", "w");
    exit_t result = serialize_symbol_table(actual_sym_file, 0x3000);
    assert_int_equal(0, result.code);
    fclose(actual_sym_file);

    //test symbol table serialization
    FILE *expected_sym_file = fopen("./test/testfiles/t2.expected.sym", "r");
    actual_sym_file = fopen("./test/testfiles/t2.sym", "r");

    size_t num_lines = 1;
    char *line_expected = NULL;
    char *line_actual = NULL;
    size_t len_expected = 0;
    size_t len_actual = 0;
    ssize_t read_expected;
    while((read_expected = getline(&line_expected, &len_expected, expected_sym_file)) != -1) {
        // printf("line checked: %zu\n", num_lines);
        ssize_t read_actual;
        if((read_actual = getline(&line_actual, &len_actual, actual_sym_file)) != -1) {
            assert_true(strcmp(line_expected, line_actual) == 0);
            num_lines++;
        } else {
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
    add("LABEL", 4);
    FILE *actual_sym_file = fopen("./test/testfiles/t2.sym", "r");
    exit_t result = serialize_symbol_table(actual_sym_file, 0x3000);
    fclose(actual_sym_file);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "error when writing serialized symbol table to file: 9");
    free(result.desc);
}


static void test_assemble_or_asm(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/or.asm", "./test/testfiles/or.expected.obj", "./test/testfiles/or.obj");
}

static void test_assemble_abs_asm(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/abs.asm", "./test/testfiles/abs.expected.obj", "./test/testfiles/abs.obj");
}

static void test_assemble_lcrng_asm(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/lcrng.asm", "./test/testfiles/lcrng.expected.obj", "./test/testfiles/lcrng.obj");
}

static void test_assemble_charcounter_asm(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/charcounter.asm", "./test/testfiles/charcounter.expected.obj", "./test/testfiles/charcounter.obj");
}

static void test_assemble_lc3os_asm(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/lc3os.asm", "./test/testfiles/lc3os.expected.obj", "./test/testfiles/lc3os.obj");
}

static void test_assemble_2048_asm(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/testfiles/2048.asm", "./test/testfiles/2048.expected.obj", "./test/testfiles/2048.obj");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_symbol_table_t2, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_t3, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_t4, setup, teardown),
        cmocka_unit_test_setup_teardown(test_two_labels_same_line_t5, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_without_labels_t1, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_with_labels_t2, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_wrong_orig_address_t6, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_missing_orig_t7, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_missing_orig_address_t8, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_stringz_t9, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_blkw_t10, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_with_labels_t11, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_with_wrong_stringz_t12, setup, teardown),
        cmocka_unit_test_setup_teardown(test_missing_assembly_file, setup, teardown),
        cmocka_unit_test_setup_teardown(test_wrong_assembly_file_extension, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_serialization, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symbol_table_serialization_failure, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_or_asm, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_abs_asm, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_lcrng_asm, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_charcounter_asm, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_lc3os_asm, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_2048_asm, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
