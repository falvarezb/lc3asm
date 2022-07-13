
#ifndef FAB_COMMON
#define FAB_COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#define ERR_DESC_LENGTH 200
extern char errdesc[];

void error_exit(const char *format, const char *text);
char *bin(unsigned int decimal);
int seterrdesc(char *format, ...);
void clearerrdesc();
bool strtolong(char *str, long *num);
char **split_tokens(char *str, int *num_tokens, const char *delimiters);

#endif
