#include "common.h"
#define ERR_DESC_LENGTH 100
char errdesc[ERR_DESC_LENGTH];

/**
 * @brief Prints error message to stdout and exits the program
 * 
 * @param format 
 * @param text 
 */
void error_exit(const char *format, const char *text) {
    printf(format, text);
    exit(EXIT_FAILURE);
}

/**
 * @brief Stores error description in global variable errdesc
 * 
 * @param s 
 * @param format 
 * @return int return value of snprintf
 */
int printerr(char *s, const char *format) {
    return snprintf(errdesc, ERR_DESC_LENGTH, s, format);
}

/**
 * @brief Calculate binary representation of a decimal number
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
