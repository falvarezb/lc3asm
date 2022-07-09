
#ifndef FAB_LC3_COMMON
#define FAB_LC3_COMMON
#include "common.h"
#include "dict.h"

typedef enum {
    ORIG_DIRECTIVE, END_DIRECTIVE, OPCODE, LABEL, COMMENT, BLANK_LINE
} linetype_t;

typedef enum {
    ADD, AND, JMP, JSR, NOT, RET, HALT
} opcode_t;

int is_register(char *token);
int is_imm5(char *token, long *imm5);
int is_valid_memaddr(char *token, long *memaddr);
uint16_t do_return(uint16_t ret, char **tokens);
char **instruction_tokens(char *asm_instr, char *instr_name, int num_tokens);
char **line_tokens(const char *line, int *num_tokens, char** line_copy_ptr);


#endif
