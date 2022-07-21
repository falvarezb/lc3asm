#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_jmp_register(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_jmp("R6",&machine_instr,0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 128);
    assert_int_equal(bytes[1], 193);
}

void test_jmp_wrong_register_BaseR(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    exit_t result = parse_jmp("R8",&machine_instr,0);

    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Expected register but found R8");
}


int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_jmp_register),
        cmocka_unit_test(test_jmp_wrong_register_BaseR)                       
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
