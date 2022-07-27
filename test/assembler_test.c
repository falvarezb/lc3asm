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

// static void assert_symbol_table(const char *label, size_t num_instruction) {
//     node_t *node = lookup(label);
//     assert_non_null(node);
//     assert_int_equal(node->val, num_instruction);    
// }

// static void run_second_pass_test(char *asm_file_name, char *expected_obj_file_name, char *actual_obj_file_name) {
//     second_pass_parse(asm_file_name, actual_obj_file_name);

//     FILE *expected_obj_file = fopen(expected_obj_file_name, "r");
//     FILE *actual_obj_file = fopen(actual_obj_file_name, "r");

//     char buf_expected[2];
//     char buf_actual[2];
//     size_t num_lines = 1;

//     size_t read;
//     while((read = fread(buf_expected, 1, 2, expected_obj_file)) == 2) {
//         read = fread(buf_actual, 1, 2, actual_obj_file);
//         printf("line checked: %zu\n", num_lines);
//         assert_int_equal(read, 2);
//         assert_true(buf_expected[0] == buf_actual[0] && buf_expected[1] == buf_actual[1]);
//         num_lines++;
//     }

//     assert_int_equal(read, fread(buf_actual, 1, 2, actual_obj_file));
//     fclose(expected_obj_file);
//     fclose(actual_obj_file);
// }

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

// static void test_second_pass_without_labels_t1(void  __attribute__((unused)) **state) {
//     run_second_pass_test("./test/t1.asm", "./test/t1.expected.obj", "./test/t1.obj");
// }

// static void test_second_pass_with_labels_t2(void  __attribute__((unused)) **state) {
//     add("LABEL", 0x3003);
//     run_second_pass_test("./test/t2.asm", "./test/t2.expected.obj", "./test/t2.obj");
// }

// static void test_symbol_table_t2(void  __attribute__((unused)) **state) {
//     compute_symbol_table("./test/t2.asm");
//     assert_symbol_table("LABEL", 0x3003);
// }

// static void test_symbol_table_t3(void  __attribute__((unused)) **state) {
//     compute_symbol_table("./test/t3.asm");
//     assert_symbol_table("LABEL", 0x3003);
// }

// static void test_symbol_table_t4(void  __attribute__((unused)) **state) {
//     compute_symbol_table("./test/t4.asm");
//     assert_symbol_table("LABEL1", 0x3003);
//     assert_symbol_table("LABEL2", 0x3001);
//     assert_symbol_table("LABEL3", 0x3002);
//     assert_symbol_table("LABEL4", 0x3004);
//     assert_symbol_table("LABEL5", 0x3003);
// }

// static void test_symbol_table_t5(void  __attribute__((unused)) **state) {
//     exit_t result = compute_symbol_table("./test/t5.asm");
//     assert_int_equal(result.code, EXIT_FAILURE);
//     assert_string_equal(result.desc, "ERROR (line 10): Invalid opcode ('LABEL2')");
//     free(result.desc);
// }

static void test_assemble_without_labels_t1(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/t1.asm", "./test/t1.expected.obj", "./test/t1.obj");
}

static void test_assemble_with_labels_t2(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/t2.asm", "./test/t2.expected.obj", "./test/t2.obj");
}

static void test_assemble_wrong_orig_address_t6(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/t6.asm");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 4): Immediate operand (545677767) outside of range (0 to 65535)");
    free(result.desc);
}

static void test_assemble_missing_orig_t7(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/t7.asm");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 4): Instruction not preceeded by a .orig directive");
    free(result.desc);
}

static void test_assemble_missing_orig_address_t8(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/t8.asm");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 4): Immediate expected");
    free(result.desc);
}

static void test_missing_assembly_file(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/test/random.asm");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR: Couldn't read file (./test/test/random.asm)");
    free(result.desc);
}

static void test_wrong_assembly_file_extension(void  __attribute__((unused)) **state) {
    exit_t result = assemble("./test/t2.copy");
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR: Input file must have .asm suffix ('./test/t2.copy')");
    free(result.desc);
}

static void test_assemble_or_asm(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/or.asm", "./test/or.expected.obj", "./test/or.obj");
}

static void test_assemble_abs_asm(void  __attribute__((unused)) **state) {
    run_assemble_test("./test/abs.asm", "./test/abs.expected.obj", "./test/abs.obj");
}


int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        // cmocka_unit_test_setup_teardown(test_second_pass_without_labels_t1, setup, teardown),
        // cmocka_unit_test_setup_teardown(test_second_pass_with_labels_t2, setup, teardown),
        // cmocka_unit_test_setup_teardown(test_symbol_table_t2, setup, teardown),
        // cmocka_unit_test_setup_teardown(test_symbol_table_t3, setup, teardown),
        // cmocka_unit_test_setup_teardown(test_symbol_table_t4, setup, teardown),
        // cmocka_unit_test_setup_teardown(test_symbol_table_t5, setup, teardown),  

        cmocka_unit_test_setup_teardown(test_assemble_without_labels_t1, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_with_labels_t2, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_wrong_orig_address_t6, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_missing_orig_t7, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_missing_orig_address_t8, setup, teardown),
        cmocka_unit_test_setup_teardown(test_missing_assembly_file, setup, teardown),
        cmocka_unit_test_setup_teardown(test_wrong_assembly_file_extension, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_or_asm, setup, teardown),
        cmocka_unit_test_setup_teardown(test_assemble_abs_asm, setup, teardown)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
