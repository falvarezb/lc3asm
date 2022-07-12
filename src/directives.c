/**
 * @file directives.c
 * @brief functions to generate machine code out of assembly directives
 * @version 0.1
 * @date 2022-07-12
 * 
 */

#include "../include/lc3.h"

uint16_t halt() {    
    return 0xf025;
}

uint16_t orig(char *token) {
    long memaddr;
    if(is_valid_memaddr(token, &memaddr)) {
        return 0;
    }
    return memaddr;
}