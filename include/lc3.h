#ifndef FAB_LC3
#define FAB_LC3
#include "lc3util.h"

exit_t parse_add(char *operand1, char* operand2, char* operand3, uint16_t *machine_instruction);
exit_t parse_and(char *asm_instr, uint16_t *machine_instr);
exit_t parse_not(char *asm_instr, uint16_t *machine_instr);
exit_t parse_ret(char *asm_instr, uint16_t *machine_instr);
exit_t parse_jmp(char *asm_instr, uint16_t *machine_instr);
exit_t parse_jsr(char *operand, uint16_t instruction_number, uint16_t *machine_instr);
exit_t halt(uint16_t *machine_instr);
exit_t orig(char *token, uint16_t *memaddr);

exit_t first_pass_parse(const char *assembly_file_name, const char *symbol_table_file_name);
exit_t second_pass_parse(const char *assembly_file_name, const char *object_file_name);
exit_t compute_symbol_table(const char *assembly_file_name);
exit_t serialize_symbol_table(const char* symbol_table_file_name);
exit_t assemble(const char *assembly_file_name);


#endif
