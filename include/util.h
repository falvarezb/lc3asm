
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

#define ERR_DESC_LENGTH 300
#define MAX_NUM_TOKENS 200
extern char errdesc[];

// passing elipsis around from a function to another is only possible with macros
#define failure(exit_code, format, ...) do_exit(exit_code, format, __VA_ARGS__)

typedef struct {
    int code;
    char *desc;
} exit_t;

typedef struct {
    char *before;
    char *after;
} split_t;

void error_exit(const char *format, ...) __printflike(1, 2);
char *bin(unsigned int decimal);
int seterrdesc(char *format, ...) __printflike(1, 2);
void clearerrdesc();
bool strtolong(char *str, long *num, int base);
char **split_tokens(char *str, int *num_tokens, const char *delimiters);
char *split_by_last_delimiter(char *str, char delimiter);
exit_t do_exit(int exit_code, char *format, ...) __printflike(2, 3);
exit_t success();
void free_err(exit_t err);

#endif
