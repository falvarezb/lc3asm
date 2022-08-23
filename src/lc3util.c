/**
 * @file lc3util.c
 * @brief util functions used during the assembly process
 * @version 0.1
 * @date 2022-07-12
 *
 */

#include "../include/lc3.h"
#define LC3_WORD_SIZE 16 // bits

 /**
  * @brief Returns numeric value of the register or -1 if `str` is not a register
  *
  * @param str
  * @return int
  */
int parse_register(char *str) {
    if(strcmp(str, "R0") == 0) {
        return 0;
    }
    else if(strcmp(str, "R1") == 0) {
        return 1;
    }
    else if(strcmp(str, "R2") == 0) {
        return 2;
    }
    else if(strcmp(str, "R3") == 0) {
        return 3;
    }
    else if(strcmp(str, "R4") == 0) {
        return 4;
    }
    else if(strcmp(str, "R5") == 0) {
        return 5;
    }
    else if(strcmp(str, "R6") == 0) {
        return 6;
    }
    else if(strcmp(str, "R7") == 0) {
        return 7;
    }
    return -1;
}

static exit_t parse_numeric_value(char *token, long *imm, uint16_t line_counter) {
    char first_ch = *token;
    char *value_to_check;
    int base;
    if(first_ch == '#') { //decimal literal
        value_to_check = token + 1;
        base = 10;
    }
    else if(first_ch == 'x') { //hex literal
        value_to_check = token + 1;
        base = 16;
    }
    else { // decimal literal without prefix
        value_to_check = token;
        base = 10;
    }

    if(!strtolong(value_to_check, imm, base)) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate %s is not a numeric value", line_counter, token);
    }
    return success();
}

exit_t is_valid_lc3integer(char *token, int16_t *imm, uint16_t line_counter) {
    long tmp;
    exit_t result = parse_numeric_value(token, &tmp, line_counter);
    if(result.code) {
        return result;
    }
    *imm = (int16_t)tmp;
    return success();
}

/**
 * @brief Transforms the given string into imm5
 *
 * imm5 is a 5-bit value, range [-16,15]
 * it can be expressed in decimal and hex notation
 *
 * @param str string to be parsed
 * @param imm5 immediate value resulting of transforming str
 * @return int 0 if parsing is successful, else 1 (errdesc is set with error details)
 */
exit_t parse_imm5(char *str, long *imm5, uint16_t line_counter) {
    exit_t result = parse_numeric_value(str, imm5, line_counter);
    if(result.code) {
        return result;
    }

    int min = -16;
    int max = 15;
    if(*imm5 < min || *imm5 > max) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate operand (%s) outside of range (%d to %d)", line_counter, str + 1, min, max);
    }
    return success();
}

exit_t parse_memory_address(char *str, long *n, uint16_t line_counter) {
    exit_t result = parse_numeric_value(str, n, line_counter);
    if(result.code) {
        return result;
    }

    int min = 0;
    int max = 0xFFFF;
    if(*n < min || *n > max) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate operand (%s) outside of range (%d to %d)", line_counter, str + 1, min, max);
    }
    return success();
}

exit_t parse_offset(char *token, int lower_bound, int upper_bound, uint16_t instruction_number, uint16_t line_counter, long *offset, int num_bits) {

    char first_ch = *token;
    char *value_to_check;
    int base;
    if(first_ch == '#') { //decimal literal
        value_to_check = token + 1;
        base = 10;
    }
    else if(first_ch == 'x') { //hex literal
        value_to_check = token + 1;
        base = 16;
    }
    else { //decimal without prefix
        value_to_check = token;
        base = 10;
    }

    //is value a label or a number?
    if(!strtolong(value_to_check, offset, base)) {
        //transform label into offset by retrieving the memory location corresponding to the label from symbol table
        node_t *node = lookup(token);
        if(!node) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Symbol not found ('%s')", line_counter, token);
        }
        *offset = node->val - instruction_number - 1;
    }


    //validate offset numerical range
    if(*offset < lower_bound || *offset > upper_bound) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Value of offset %ld is outside the range [%d, %d]", line_counter, *offset, lower_bound, upper_bound);
    }


    //n-bit 2's complement
    if(*offset < 0) {
        *offset += (1 << num_bits);
    }

    return success();
}

exit_t parse_trapvector(char *token, long *trapvector, uint16_t line_counter) {

    exit_t result = parse_numeric_value(token, trapvector, line_counter);
    if(result.code) {
        return result;
    }

    //validate trapvector numerical range
    if(*trapvector < 0 || *trapvector > 255) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Value of trapvector %ld is outside the range [0, 255]", line_counter, *trapvector);
    }

    return success();
}

/**
 * @brief Determine the type of a line based on the value of the first token
 *
 * @param first_token first token of the line being parsed
 * @return linetype_t value indicative of the type of line
 */
linetype_t compute_line_type(const char *first_token) {

    linetype_t result;
    if(first_token[0] == '\n') {
        result = BLANK_LINE;
    }
    else if(
        strcmp(first_token, "ADD") == 0 ||
        strcmp(first_token, "AND") == 0 ||
        strcmp(first_token, "JMP") == 0 ||
        strcmp(first_token, "JMPT") == 0 ||
        strcmp(first_token, "JSR") == 0 ||
        strcmp(first_token, "JSRR") == 0 ||
        strcmp(first_token, "NOT") == 0 ||
        strcmp(first_token, "RET") == 0 ||
        strcmp(first_token, "RTT") == 0 ||
        strcmp(first_token, "GETC") == 0 ||
        strcmp(first_token, "OUT") == 0 ||
        strcmp(first_token, "PUTS") == 0 ||
        strcmp(first_token, "IN") == 0 ||
        strcmp(first_token, "PUTSP") == 0 ||
        strcmp(first_token, "HALT") == 0 ||
        strcmp(first_token, "LD") == 0 ||
        strcmp(first_token, "ST") == 0 ||
        strcmp(first_token, "LDI") == 0 ||
        strcmp(first_token, "STI") == 0 ||
        strcmp(first_token, "LEA") == 0 ||
        strcmp(first_token, "BR") == 0 ||
        strcmp(first_token, "BRnzp") == 0 ||
        strcmp(first_token, "BRnz") == 0 ||
        strcmp(first_token, "BRnp") == 0 ||
        strcmp(first_token, "BRzp") == 0 ||
        strcmp(first_token, "BRn") == 0 ||
        strcmp(first_token, "BRz") == 0 ||
        strcmp(first_token, "BRp") == 0 ||
        strcmp(first_token, "LDR") == 0 ||
        strcmp(first_token, "STR") == 0 ||
        strcmp(first_token, "RTI") == 0 ||
        strcmp(first_token, "TRAP") == 0
        ) {
        result = OPCODE;
    }
    else if(strcmp(first_token, ".ORIG") == 0) {
        result = ORIG_DIRECTIVE;
    }
    else if(strcmp(first_token, ".END") == 0) {
        result = END_DIRECTIVE;
    }
    else if(strcmp(first_token, ".FILL") == 0) {
        result = FILL_DIRECTIVE;
    }
    else if(strcmp(first_token, ".BLKW") == 0) {
        result = BLKW_DIRECTIVE;
    }
    else if(strcmp(first_token, ".STRINGZ") == 0) {
        result = STRINGZ_DIRECTIVE;
    }
    else if(first_token[0] == ';') {
        result = COMMENT;
    }
    else {
        //any unrecognised token is considered to be a label
        result = LABEL;
    }

    return result;
}

/**
 * @brief Determine the opcode of an instruction
 *
 * @param opcode opcode to be analyzed
 * @return opcode_t value indicative of the type of opcode
 */
opcode_t compute_opcode_type(const char *opcode) {

    opcode_t result;
    if(strcmp(opcode, "ADD") == 0) {
        result = ADD;
    }
    else if(strcmp(opcode, "AND") == 0) {
        result = AND;
    }
    else if(strcmp(opcode, "JMP") == 0) {
        result = JMP;
    }
    else if(strcmp(opcode, "JMPT") == 0) {
        result = JMPT;
    }
    else if(strcmp(opcode, "JSR") == 0) {
        result = JSR;
    }
    else if(strcmp(opcode, "NOT") == 0) {
        result = NOT;
    }
    else if(strcmp(opcode, "RET") == 0) {
        result = RET;
    }
    else if(strcmp(opcode, "RTT") == 0) {
        result = RTT;
    }
    else if(strcmp(opcode, "LD") == 0) {
        result = LD;
    }
    else if(strcmp(opcode, "ST") == 0) {
        result = ST;
    }
    else if(strcmp(opcode, "LDI") == 0) {
        result = LDI;
    }
    else if(strcmp(opcode, "STI") == 0) {
        result = STI;
    }
    else if(strcmp(opcode, "LEA") == 0) {
        result = LEA;
    }
    else if(strcmp(opcode, "BR") == 0) {
        result = BR;
    }
    else if(strcmp(opcode, "BRnzp") == 0) {
        result = BRnzp;
    }
    else if(strcmp(opcode, "BRn") == 0) {
        result = BRn;
    }
    else if(strcmp(opcode, "BRz") == 0) {
        result = BRz;
    }
    else if(strcmp(opcode, "BRp") == 0) {
        result = BRp;
    }
    else if(strcmp(opcode, "BRnz") == 0) {
        result = BRnz;
    }
    else if(strcmp(opcode, "BRnp") == 0) {
        result = BRnp;
    }
    else if(strcmp(opcode, "BRzp") == 0) {
        result = BRzp;
    }
    else if(strcmp(opcode, "GETC") == 0) {
        result = GETC;
    }
    else if(strcmp(opcode, "OUT") == 0) {
        result = OUT;
    }
    else if(strcmp(opcode, "PUTS") == 0) {
        result = PUTS;
    }
    else if(strcmp(opcode, "IN") == 0) {
        result = IN;
    }
    else if(strcmp(opcode, "PUTSP") == 0) {
        result = PUTSP;
    }
    else if(strcmp(opcode, "HALT") == 0) {
        result = HALT;
    }
    else if(strcmp(opcode, "JSRR") == 0) {
        result = JSRR;
    }
    else if(strcmp(opcode, "LDR") == 0) {
        result = LDR;
    }
    else if(strcmp(opcode, "STR") == 0) {
        result = STR;
    }
    else if(strcmp(opcode, "RTI") == 0) {
        result = RTI;
    }
    else if(strcmp(opcode, "TRAP") == 0) {
        result = TRAP;
    }
    else {
        assert(false);
    }
    return result;
}

void free_line_metadata(linemetadata_t *line_metadata) {
    if(line_metadata->tokens) {
        if(line_metadata->is_label_line) {
            free(line_metadata->tokens - 1);
        }
        else {
            free(line_metadata->tokens);
        }
        free(line_metadata->line);
    }
    free(line_metadata);
}

void free_tokenized_lines(linemetadata_t **tokenized_lines) {
    linemetadata_t *line_metadata;
    memaddr_t address_offset = 0;
    while((line_metadata = tokenized_lines[address_offset++])) {
        free_line_metadata(line_metadata);
    }
    free(tokenized_lines);
}

