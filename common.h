
#ifndef FAB_COMMON
#define FAB_COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#define LC3_WORD_SIZE 16 // bits


/**
 * @brief Prints error message and exits the program
 * 
 * @param format 
 * @param text 
 */
void error_exit(const char *format, const char *text) {
    printf(format, text);
    exit(EXIT_FAILURE);
}

/**
 * @brief 
 * Calculate binary representation of a decimal number
 * 
 * @param decimal 
 * @return char* containing binary representation
 */
char *bin(unsigned int decimal) {
    size_t size = CHAR_BIT * sizeof(int);
    char *result = (char *)malloc(size);
    if(result == NULL) {
        error_exit("failure to allocate memory", "");
    }

    size_t i = size;
    while(i--) {
        *(result + (size - i - 1)) = ((decimal >> i) & 1);
    }
    return result;
}


#endif