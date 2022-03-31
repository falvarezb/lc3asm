#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "lc3.h"

//extern char *errdesc;

void test_add_register(void **state) {
    char asm_instr[] = "ADD R0,R1,R2";
    uint16_t machine_instr = parse_add(asm_instr);
    char *bytes = (char*) &machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 66);
    assert_int_equal(bytes[1], 16);
}

void test_add_imm5_decimal(void **state) {
    char asm_instr[] = "ADD R0,R1,#13";
    uint16_t machine_instr = parse_add(asm_instr);
    char *bytes = (char*) &machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 109);
    assert_int_equal(bytes[1], 16);
}

void test_add_imm5_hex(void **state) {
    char asm_instr[] = "ADD R0,R1,xa";
    uint16_t machine_instr = parse_add(asm_instr);
    char *bytes = (char*) &machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 106);
    assert_int_equal(bytes[1], 16);
}

void test_wrong_register_DR(void **state) {
    char asm_instr[] = "ADD R8,R1,xa";
    uint16_t machine_instr = parse_add(asm_instr);
    
    assert_int_equal(machine_instr, 0);
    assert_string_equal(errdesc, "expected register but found R8\n");
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add_register),
        cmocka_unit_test(test_add_imm5_decimal),
        cmocka_unit_test(test_add_imm5_hex),
        cmocka_unit_test(test_wrong_register_DR),
        };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
