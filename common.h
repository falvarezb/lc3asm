
#ifndef FAB_COMMON
#define FAB_COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

extern char errdesc[];

void error_exit(const char *format, const char *text);
char *bin(unsigned int decimal);
int printerr(char *s, const char *format);

#endif