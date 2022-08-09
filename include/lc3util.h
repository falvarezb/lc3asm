
#ifndef FAB_LC3_COMMON
#define FAB_LC3_COMMON
#include "util.h"
#include "dict.h"

#define ADDRESS_SPACE_CARDINALITY 65536

typedef enum {
    ORIG_DIRECTIVE, END_DIRECTIVE, OPCODE, LABEL, COMMENT, BLANK_LINE, FILL_DIRECTIVE, BLKW_DIRECTIVE, STRINGZ_DIRECTIVE
} linetype_t;

typedef enum {
    ADD, AND, JMP, JSR, JSRR, NOT, RET, LD, ST, LDI, STI, LEA, BR, BRp, BRz, BRn, BRzp, BRnp, BRnz, BRnzp, LDR, STR, RTI, TRAP, HALT, GETC, OUT, PUTS, IN, PUTSP
} opcode_t;

typedef uint16_t memaddr_t;
typedef struct linemetadata {
    char *line;
    char **tokens; /**< tokens the line is split into; initial label, if any, is not included */
    int num_tokens;
    bool is_label_line; /**< flag to identify lines that begin with a label */
    int line_number; /**< line number inside the assembly file */
    int instruction_location; /**< instruction position inside the assembly file */
    uint16_t machine_instruction; /**< binary representation of the instruction contained by the line */
} linemetadata_t;

int parse_register(char *token);
exit_t parse_imm5(char *str, long *imm5, uint16_t line_counter);
exit_t parse_memory_address(char *str, long *n, uint16_t line_counter);
char **instruction_tokens(char *asm_instr, char *instr_name, int num_tokens);
exit_t parse_offset(char* value, int lower_bound, int upper_bound, uint16_t instruction_number, uint16_t line_counter, long *offset, int num_bits);
exit_t parse_trapvector(char *token,  long *trapvector, uint16_t line_counter);
linetype_t compute_line_type(const char *first_token);
opcode_t compute_opcode_type(const char *opcode);
void free_line_metadata(linemetadata_t *line_metadata);
void free_tokenized_lines(linemetadata_t *tokenized_lines[]);


#endif
