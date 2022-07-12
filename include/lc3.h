#ifndef FAB_LC3
#define FAB_LC3
#include "lc3util.h"

uint16_t parse_add(char *operand1, char* operand2, char* operand3);
uint16_t parse_and(char *instr);
uint16_t parse_not(char *instr);
uint16_t parse_ret(char *instr);
uint16_t parse_jmp(char *instr);
uint16_t parse_jsr(char *operand, uint16_t instruction_number);
int first_pass_parse(FILE *source_file, FILE *destination_file);
int second_pass_parse(FILE *source_file, FILE *object_file);
int compute_symbol_table(FILE *source_file);
int serialize_symbol_table(FILE* destination_file);
int assemble(FILE *source_file, FILE *symbol_table_file, FILE *object_file);


#endif
