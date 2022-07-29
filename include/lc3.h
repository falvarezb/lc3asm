#ifndef FAB_LC3
#define FAB_LC3
#include "lc3util.h"

exit_t parse_add_and(linemetadata_t *line_metadata, opcode_t opcode);
exit_t parse_not(linemetadata_t *line_metadata);
exit_t parse_jmp(linemetadata_t *line_metadata);
exit_t parse_jsr(linemetadata_t *line_metadata);
exit_t parse_br(linemetadata_t *line_metadata, int condition_codes);
exit_t parse_pc_relative_addressing_mode(linemetadata_t *line_metadata, opcode_t opcode);

exit_t serialize_symbol_table(FILE *symbol_table_file, memaddr_t address_origin);
exit_t assemble(const char *assembly_file_name);

exit_t do_lexical_analysis(FILE *assembly_file, linemetadata_t *tokenized_lines[]);
exit_t do_syntax_analysis(linemetadata_t *tokenized_lines[]);

exit_t parse_orig(linemetadata_t *line_metadata);
exit_t parse_fill(linemetadata_t *line_metadata);
exit_t is_valid_lc3integer(char *token, long *imm, uint16_t line_counter);


#endif
