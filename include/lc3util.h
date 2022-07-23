
#ifndef FAB_LC3_COMMON
#define FAB_LC3_COMMON
#include "common.h"
#include "dict.h"

typedef enum {
    ORIG_DIRECTIVE, END_DIRECTIVE, OPCODE, LABEL, COMMENT, BLANK_LINE, FILL_DIRECTIVE
} linetype_t;

typedef enum {
    ADD, AND, JMP, JSR, NOT, RET, HALT, LD, ST, LDI, STI, LEA, BR, BRp, BRz, BRn, BRzp, BRnp, BRnz
} opcode_t;

int is_register(char *token);
exit_t is_imm5(char *str, uint16_t *imm5, uint16_t line_counter);
exit_t is_valid_16bit_int(char *str, uint16_t *n, uint16_t line_counter);
uint16_t do_return(uint16_t ret, char **tokens);
char **instruction_tokens(char *asm_instr, char *instr_name, int num_tokens);
exit_t validate_offset(char* value, int lower_bound, int upper_bound, uint16_t instruction_number, uint16_t line_counter, long *offset);


#endif
