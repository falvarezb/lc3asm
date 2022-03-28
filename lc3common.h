
#ifndef FAB_LC3_COMMON
#define FAB_LC3_COMMON
#include "common.h"
#define NO_IMM5_VALUE 16


int is_register(char *token);
int is_imm5(char *token);
char *bin_word(unsigned int decimal);


#endif
