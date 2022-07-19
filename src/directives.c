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

/**
 * @brief Parse the operand of the .ORIG directive 
 * 
 * @param str operand representing starting address of the program
 * @return uint16_t numeric representation of the address or 0 if it cannot be parsed to a number
 */
uint16_t orig(char *str) {
    long memaddr;
    if(is_valid_memaddr(str, &memaddr)) {
        return 0;
    }
    return memaddr;
}

/**
 * @brief Parse the operand of the .FILL directive
 * 
 * `.FILL n` allocate one word and initialize with value n
 * 
 * @param str operand representing the value to be allocated
 * @return uint16_t numeric representation of the value or 0 if it cannot be parsed to a number
 */
// uint16_t fill(char *str, ) {

// }