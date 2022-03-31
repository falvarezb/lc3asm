
#ifndef FAB_COMMON
#define FAB_COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

#define LC3_WORD_SIZE 16 // bits
#define ERR_DESC_LENGTH 100
char errdesc[ERR_DESC_LENGTH];


void error_exit(const char *format, const char *text);
char *bin(unsigned int decimal);

#endif
