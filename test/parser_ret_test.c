#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_ret_right_instruction(void  __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_ret(&machine_instr);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 192);
    assert_int_equal(bytes[1], 193);
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_ret_right_instruction)            
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
