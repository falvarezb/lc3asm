#ifndef FAB_DICT
#define FAB_DICT

#include <stdbool.h>
#include <unistd.h>
#define DICTSIZE 101

/*
    - each key-val pair is stored in a node of a linked list
    - each linked list is pointed to by the element of an array
    - the array is indexed by the hash value of the keys
*/

//FIXME make val size_t
typedef struct node {
    struct node *next;
    char *key; 
    size_t val;
} node_t;

/**
 * Adds a new key-val pair or update the value of an existing key
 * 
 * Returns a pointer to the key-val pair created/modified or NULL if there is no 
 * enough memory for a new entry
 **/
node_t* add(const char *key, int val);

/**
 *  Returns a pointer to key-val pair or NULL if 'key' is not found
 **/
node_t* lookup(const char *key);

/**
 * Iterates over the content of the dictionary, returning a pointer 
 * to the next key-val pair on each call (or NULL if no more elements are available)
 * 
 * The state of the iterator can be reset to the first element by passing 'true' as an
 * argument
 **/
node_t* next(bool reset);

/**
 *  Deletes a key-val pair
 **/
bool delete(const char *key);

/**
 *  Initialize the elements of dictionary to null
 **/
void initialize();

/**
 * Prints out the elements of the dictionary
 **/
void print();

#endif
