#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "../include/lc3.h"

static void test_br(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_br("1", 7,0, &machine_instr,0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 14);
}

static void test_brp(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_br("1", 1,0, &machine_instr,0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 2);
}

static void test_brz(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_br("1", 2,0, &machine_instr,0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 4);
}

static void test_brn(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_br("1", 4,0, &machine_instr,0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 8);
}

static void test_brzp(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_br("1", 3,0, &machine_instr,0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 6);
}

static void test_brnp(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_br("1", 5,0, &machine_instr,0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 10);
}

static void test_brnz(void __attribute__ ((unused)) **state) {    
    uint16_t machine_instr;
    parse_br("1", 6,0, &machine_instr,0);
    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 12);
}

static void test_br_PCoffset9_too_big(void __attribute__ ((unused))  **state) {    
    uint16_t machine_instr;
    exit_t result = parse_br("300",1, 0,&machine_instr,0);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Value of offset 300 is outside the range [-256, 255]");
}

static void test_br_PCoffset9_too_small(void __attribute__ ((unused))  **state) {    
    uint16_t machine_instr;
    exit_t result = parse_br("-300",1, 0,&machine_instr,0);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Value of offset -300 is outside the range [-256, 255]");
}


static void test_br_with_label(void __attribute__ ((unused))  **state) {
    initialize();
    add("LABEL", 0x3003);    
    uint16_t machine_instr;
    parse_br("LABEL",7, 0x3001,&machine_instr,0);

    unsigned char *bytes = (unsigned char *)&machine_instr;
    //assert order is flipped because of little-endian arch
    assert_int_equal(bytes[0], 1);
    assert_int_equal(bytes[1], 14);
    initialize();
}

static void test_br_non_existent_label(void __attribute__ ((unused))  **state) {
    initialize();    
    uint16_t machine_instr;
    exit_t result = parse_br("NON_EXISTENT_LABEL",1, 0,&machine_instr,0);
    assert_int_equal(result.code, 1);
    assert_string_equal(result.desc, "ERROR (line 0): Symbol not found ('NON_EXISTENT_LABEL')");
}

int main(int __attribute__ ((unused)) argc, char const __attribute__ ((unused)) *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_br),
        cmocka_unit_test(test_brp),
        cmocka_unit_test(test_brz),
        cmocka_unit_test(test_brn),
        cmocka_unit_test(test_brzp),
        cmocka_unit_test(test_brnp),
        cmocka_unit_test(test_brnz),
        cmocka_unit_test(test_br_PCoffset9_too_big),
        cmocka_unit_test(test_br_PCoffset9_too_small),             
        cmocka_unit_test(test_br_with_label),
        cmocka_unit_test(test_br_non_existent_label)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
