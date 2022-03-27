#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include "myfile.h"

void test_add(void **state) {
    assert_string_equal("hello world", hello_world());
}

int main(int argc, char const *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add)
        };
    return cmocka_run_group_tests(tests, NULL, NULL);
}