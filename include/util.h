
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

typedef struct split_t {
    char *before;
    char *after;
} split_t;

typedef struct exit_t {
    int code;
    char *desc;
} exit_t;

void error_exit(const char *format, const char *text);
char *bin(unsigned int decimal);
int seterrdesc(char *format, ...);
void clearerrdesc();
bool strtolong(char *str, long *num);
char **split_tokens(char *str, int *num_tokens, const char *delimiters);
char *split_by_last_delimiter(char *str, char delimiter);
exit_t do_exit(int exit_code, char *format, ...);
exit_t success();
void free_err(exit_t err);

#endif
