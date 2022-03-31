
#ifndef FAB_LC3_COMMON
#define FAB_LC3_COMMON
#include "common.h"
#define NO_IMM5_VALUE 16


int is_register(char *token);
int is_imm5(char *token, int *imm5);
char *bin_lc3word(unsigned int decimal);


#endif
