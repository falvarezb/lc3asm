
#ifndef FAB_LC3_COMMON
#define FAB_LC3_COMMON
#include "common.h"


int is_register(char *token);
int is_imm5(char *token, int *imm5);
char **instruction_tokens(char *asm_instr, char *instr_name, int num_tokens);


#endif
