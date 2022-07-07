#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/dict.h"

static node_t *dict[DICTSIZE];

unsigned hash(const char *s) {
    unsigned hashval;
    for(hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % DICTSIZE;
}

node_t *lookup(const char *key) {
    node_t *np;

    for(np = dict[hash(key)]; np != NULL; np = np->next) {
        if(strcmp(np->key, key) == 0)
            return np;
    }
    return NULL;
}

node_t *add(const char *key, uint16_t val) {
    node_t *np;
    unsigned hashval;

    if((np = lookup(key)) == NULL) {
        np = malloc(sizeof(*np));
        if(np == NULL || (np->key = strdup(key)) == NULL)
            return NULL;

        //adding new node to the front of the linked list
        hashval = hash(key);
        np->next = dict[hashval];
        dict[hashval] = np;

    }

    np->val = val;

    return np;
}

bool delete(const char *key) {

    node_t *curr, *prev = NULL;

    int hashval = hash(key);
    for(curr = dict[hashval]; curr != NULL; prev = curr, curr = curr->next) {
        if(strcmp(curr->key, key) == 0) {
            if(prev == NULL) {
                //remove node from front of list
                dict[hashval] = curr->next;
            }
            else {
                prev->next = curr->next;
            }
            free((void *)curr->key);
            free((void *)curr);
            return true;
        }
    }

    return false;
}

void print() {
    node_t *np;

    for(size_t i = 0; i < DICTSIZE; i++) {
        int has_elements = 0;
        for(np = dict[i]; np != NULL; np = np->next) {
            has_elements = 1;
            if(np == dict[i])
                printf("%zu ", i);
            printf("- (%s,%hu) ", np->key, np->val);
        }
        if(has_elements)
            printf("\n");
    }
}

node_t *next(bool reset) {
    node_t *result = NULL;
    static size_t i = 0;
    static node_t *current = NULL;

    if(reset) {
        i = 0;
        current = dict[0];
    }

    while(current == NULL && i < DICTSIZE - 1) {
        i++;
        current = dict[i];
    }
    result = current;
    
    if(current) {
        current = current->next;
    }
    return result;
}

void initialize() {
    node_t *node = next(true);
    while(node) {
        delete(node->key);
        node = next(false);
    }
}
