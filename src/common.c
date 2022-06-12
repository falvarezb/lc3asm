#include "../include/common.h"
#define ERR_DESC_LENGTH 200
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
 * @param format
 * @param ... format specifiers
 * @return int return value of vsnprintf
 */ 
int printerr(char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(errdesc, ERR_DESC_LENGTH, format, ap);
    va_end(ap);
    return result;
}

void clearerrdesc() {    
    errdesc[0] = '\0';
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

void strtolong(char *str, long **num) {
    char *pEnd;
    long l = strtol(str, &pEnd, 10);
    if(str == pEnd) {
        *num = NULL;
    }
    else {
        **num = l;
    }

}
