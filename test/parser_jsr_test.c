#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

void test_jsr_right_PCoffset11(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_jsr("1", 0, &machine_instr);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 72);
}

void test_jsr_PCoffset11_too_big(void __attribute__ ((unused))  **state) {    
    uint16_t machine_instr;
    exit_t result = parse_jsr("2000", 0,&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "value of PCoffset11 2000 is outside the range [-1024, 1023]");
}

void test_jsr_PCoffset11_too_small(void __attribute__ ((unused))  **state) {    
    uint16_t machine_instr;
    exit_t result = parse_jsr("-2000", 0,&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "value of PCoffset11 -2000 is outside the range [-1024, 1023]");
}


void test_jsr_with_label(void __attribute__ ((unused))  **state) {
    initialize();
    add("LABEL", 0x3003);    
    uint16_t machine_instr;
    parse_jsr("LABEL", 0x3001,&machine_instr);

    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 72);
    initialize();
}

void test_jsr_non_existent_label(void __attribute__ ((unused))  **state) {
    initialize();    
    uint16_t machine_instr;
    exit_t result = parse_jsr("NON_EXISTENT_LABEL", 0,&machine_instr);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "Symbol not found ('NON_EXISTENT_LABEL')");
}

int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused)) *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_jsr_right_PCoffset11),
        cmocka_unit_test(test_jsr_PCoffset11_too_big),
        cmocka_unit_test(test_jsr_PCoffset11_too_small),             
        cmocka_unit_test(test_jsr_with_label),
        cmocka_unit_test(test_jsr_non_existent_label)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
