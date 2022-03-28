#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "lc3.h"


void test_add(void **state) {
    char asm_instr[] = "ADD R0,R1,R2";
    uint16_t *machine_instr = parse_add(asm_instr);
    char *bytes = (char*)machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 66);
    assert_int_equal(bytes[1], 16);
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add)
        };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
