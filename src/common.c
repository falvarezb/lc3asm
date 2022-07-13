/**
 * @file common.c
 * @brief Generic util functions
 * @version 0.1
 * @date 2022-07-12
 * 
 */

#include "../include/common.h"
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
int seterrdesc(char *format, ...) {
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
    char *result = malloc(size);
    if(result == NULL) {
        error_exit("failure to allocate memory", "");
    }

    size_t i = size;
    while(i--) {
        *(result + (size - i - 1)) = ((decimal >> i) & 1);
    }
    return result;
}

/**
 * @brief Converts string into long and stores the value into num
 * 
 * @param str string representing a number
 * @param num pointer to store resulting number
 * @return bool false if string cannot be converted to a number, true otherwise
 */
bool strtolong(char *str, long *num) {
    char *pEnd;
    long l = strtol(str, &pEnd, 10);
    if(str == pEnd) {
        return false;
    }
    else {
        *num = l;
        return true;
    }
}

/**
 * @brief Splits the given string into tokens
 * 
 * This function generates an array of pointers, each pointing to the location of `str` corresponding to the
 * beginning of that token.
 * Beware that `str` is mangled.
 * 
 * Tokens are delimited by the characters in `delimiters`.
 * 
 * @param str string to parse
 * @param num_tokens pointer used to store the number of tokens found
 * @return char** array of pointers to each of the tokens found, size of the array is given by num_tokens
 */
char **split_tokens(char *str, int *num_tokens, const char *delimiters) {
    const int max_num_tokens = 200;
    char **tokens = malloc(max_num_tokens * sizeof(char *));
    if(tokens == NULL) {
        seterrdesc("out of memory\n");
        return NULL;
    }

    *num_tokens = 0;    
    char *pch = strtok(str, delimiters);
    while(pch != NULL) {
        tokens[(*num_tokens)++] = pch;
        pch = strtok(NULL, delimiters);
    }

    if(*num_tokens == 0) {
        free(tokens);        
        return NULL;
    }

    char** resized_tokens = realloc(tokens, *num_tokens * sizeof(char *));
    if(resized_tokens == NULL) {
        seterrdesc("out of memory\n");        
        free(tokens);
        return NULL;
    }
    
    return resized_tokens;
}

