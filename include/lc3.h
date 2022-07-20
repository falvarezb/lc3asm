#ifndef FAB_LC3
#define FAB_LC3
#include "lc3util.h"

exit_t parse_add(char *operand1, char* operand2, char* operand3, uint16_t *machine_instruction);
exit_t parse_and(char *asm_instr, uint16_t *machine_instr);
uint16_t parse_not(char *instr);
uint16_t parse_ret(char *instr);
uint16_t parse_jmp(char *instr);
uint16_t parse_jsr(char *operand, uint16_t instruction_number);
uint16_t halt();
uint16_t orig(char *token);

int first_pass_parse(const char *assembly_file_name, const char *symbol_table_file_name);
int second_pass_parse(const char *assembly_file_name, const char *object_file_name);
int compute_symbol_table(const char *assembly_file_name);
int serialize_symbol_table(const char* symbol_table_file_name);
int assemble(const char *assembly_file_name);


#endif
