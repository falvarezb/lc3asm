#include "../include/dict.h"
#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

void add_new_entry(void** state){
    (void) state; /* unused */

    node_t* entry = add("mykey", 1);
    assert_non_null(entry);
    assert_string_equal(entry->key, "mykey");
    assert_int_equal(entry->val, 1);
    print();
}

void lookup_entry(void** state){
    (void) state; /* unused */

    node_t* entry = lookup("mykey");
    assert_non_null(entry);
    assert_string_equal(entry->key, "mykey");
    assert_int_equal(entry->val, 1);
}

void lookup_nonexistent_entry(void** state){
    (void) state; /* unused */

    node_t* entry = lookup("randomkey");
    assert_null(entry);
}

void modify_existing_entry(void** state){
    (void) state; /* unused */

    node_t* entry = add("mykey", 2);      
    assert_string_equal(entry->key, "mykey");
    assert_int_equal(entry->val, 2);
    print();
}

void remove_only_existing_entry(void** state){
    (void) state; /* unused */
    
    bool deleted = delete("mykey");
    assert_true(deleted);
    node_t* removed_entry = lookup("mykey");
    assert_null(removed_entry);  
    print();    
}

void remove_first_entry(void** state){
    (void) state; /* unused */
    
    //'fc' and 'key' have the same hash value
    add("fc", 3);
    add("key", 1);
    print();
    bool deleted = delete("key");  
    assert_true(deleted); 
    node_t* entry = lookup("key");
    assert_null(entry);  
    entry = lookup("fc");
    assert_non_null(entry);  
    print(); 
}

void remove_last_entry(void** state){
    (void) state; /* unused */
    
    add("key", 1);
    print();
    bool deleted = delete("fc");   
    assert_true(deleted);
    node_t* entry = lookup("fc");
    assert_null(entry);  
    entry = lookup("key");
    assert_non_null(entry);  
    print(); 
}

void fail_remove_nonexistent_entry(void** state){
    (void) state; /* unused */
    
    print();
    bool deleted = delete("xxx");   
    assert_false(deleted);    
}

void printer(void** state){
    (void) state; /* unused */
    
    add("fc", 2);
    add("key", 1);
    add("flc", 3);
    add("kuey", 4);
    print(); 
}

void iterate_over_dictionary(void** state){
    (void) state; /* unused */
    
    assert_true(DICTSIZE > 65);
    initialize();
    add("fc", 2);
    add("key", 1);
    add("flc", 3);
    add("kuey", 4);
    print();

    node_t* entry = next(true);
    assert_string_equal(entry->key, "kuey");
    assert_int_equal(entry->val, 4);

    entry = next(false);
    assert_string_equal(entry->key, "key");
    assert_int_equal(entry->val, 1);

    entry = next(false);
    assert_string_equal(entry->key, "fc");
    assert_int_equal(entry->val, 2);

    entry = next(false);
    assert_string_equal(entry->key, "flc");
    assert_int_equal(entry->val, 3);

    entry = next(false);
    assert_null(entry);

    entry = next(true);
    assert_string_equal(entry->key, "kuey");
    assert_int_equal(entry->val, 4);     
}

void initialize_dictionary(void** state){
    (void) state; /* unused */
    
    assert_true(DICTSIZE > 65);
    initialize();
    add("kuey", 4);
    print();

    node_t* entry = next(true);
    assert_string_equal(entry->key, "kuey");
    assert_int_equal(entry->val, 4);

    initialize();

    entry = next(true);
    assert_null(entry);        
}

int main(int argc, char const *argv[])
{
    (void) argv; /* unused */
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(add_new_entry),
        cmocka_unit_test(lookup_entry),
        cmocka_unit_test(lookup_nonexistent_entry),
        cmocka_unit_test(modify_existing_entry),
        cmocka_unit_test(remove_only_existing_entry),
        cmocka_unit_test(remove_first_entry),
        cmocka_unit_test(remove_last_entry),
        cmocka_unit_test(fail_remove_nonexistent_entry),
        cmocka_unit_test(iterate_over_dictionary),
        cmocka_unit_test(initialize_dictionary),
        cmocka_unit_test(printer)        
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
