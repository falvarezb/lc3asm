/**
 * @file lc3util.c
 * @brief util functions used during the assembly process
 * @version 0.1
 * @date 2022-07-12
 * 
 */

#include "../include/lc3util.h"
#define LC3_WORD_SIZE 16 // bits

/**
 * @brief Returns numeric value of the register or -1 if `str` is not a register
 * 
 * @param str 
 * @return int 
 */
int is_register(char *str) {
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

static exit_t parse_numeric_value(char *token, long *imm, long min, long max, uint16_t line_counter) {
    char first_ch = *token;
    if(first_ch == '#') { //decimal literal
        if(!strtolong(token + 1, imm)) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate %s is not a numeric value", line_counter, token);
        }
        if(*imm < min || *imm > max) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate operand (%s) outside of range (%ld to %ld)", line_counter, token + 1, min, max);            
        }        
        return success();
    }
    else if(first_ch == 'x') { //hex literal
        if(sscanf(token + 1, "%lx", imm) < 1) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Error while reading immediate %s", line_counter, token);            
        }
        if(*imm < min || *imm > max) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate operand (%s) outside of range (%ld to %ld)", line_counter, token + 1, min, max);            
        }        
        return success();
    }
    return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate %s must be decimal or hex", line_counter, token);    
}

exit_t is_valid_lc3integer(char *token, int16_t *imm, uint16_t line_counter) {
    long tmp;
    exit_t result = parse_numeric_value(token, &tmp, -32768, 32767, line_counter);    
    if(result.code) {
        return result;
    }
    *imm = (int16_t) tmp;
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
exit_t is_imm5(char *str, long *imm5, uint16_t line_counter) {
    return parse_numeric_value(str, imm5, -16, 15, line_counter);
}

exit_t is_valid_memory_address(char *str, memaddr_t *n, uint16_t line_counter) {    
    long tmp;
    exit_t result = parse_numeric_value(str, &tmp, 0, 0xFFFF, line_counter);    
    if(result.code) {
        return result;
    }
    *n = (memaddr_t) tmp;
    return success();
}

uint16_t do_return(uint16_t ret, char **tokens) {
    free(tokens);
    return ret;
}

char **instruction_tokens(char *asm_instr, char *instr_name, int num_tokens) {
    char **tokens = malloc(num_tokens * sizeof(char *));
    if(tokens == NULL) {
        seterrdesc("out of memory\n");
        return NULL;
    }
    int i = 0;
    char *delimiters = " ,";
    char *pch = strtok(asm_instr, delimiters);
    while(pch != NULL) {
        if(i > num_tokens - 1) {
            if(pch[0] == ';') {
                //comments at the end of line are ignored
                return tokens;
            }

            seterrdesc("unexpected token in %s instruction\n", instr_name);
            free(tokens);
            return NULL;
        }

        tokens[i++] = pch;
        pch = strtok(NULL, delimiters);
    }
    return tokens;
}

exit_t validate_offset(char* value, int lower_bound, int upper_bound, uint16_t instruction_number, uint16_t line_counter, long *offset) {

    //is value a label or a number?
    if(!strtolong(value, offset)) {
        //transform label into offset by retrieving the memory location corresponding to the label from symbol table
        node_t *node = lookup(value);
        if(!node) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Symbol not found ('%s')", line_counter, value);
        }
        *offset = node->val - instruction_number - 1;
    }

    //validate offset numerical range
    if(*offset < lower_bound || *offset > upper_bound) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Value of offset %ld is outside the range [%d, %d]", line_counter, *offset, lower_bound, upper_bound);
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
        strcmp(first_token, "JSR") == 0 ||
        strcmp(first_token, "NOT") == 0 ||
        strcmp(first_token, "RET") == 0 ||
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
        strcmp(first_token, "BRp") == 0
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
    else if(strcmp(opcode, "JSR") == 0) {
        result = JSR;
    }
    else if(strcmp(opcode, "NOT") == 0) {
        result = NOT;
    }
    else if(strcmp(opcode, "RET") == 0) {
        result = RET;
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
    else if(strcmp(opcode, "BR") == 0 || strcmp(opcode, "BRnzp") == 0) {
        result = BR;
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
    else if(strcmp(opcode, "HALT") == 0){
        result = HALT;
    }
    else {
        assert(false);
    }
    return result;
}

// void free_line_metadata(linemetadata_t *tokenized_lines[]) {
//     for(size_t i = 0; i < NUM_LINES; i++) {
//         if(tokenized_lines[i]) {
//             linemetadata_t *line_data = tokenized_lines[i];
//             if(line_data->is_label_line) {                               
//                 free(line_data->tokens-1);
//                 free(line_data->line);
//                 free(line_data);                
//             }
//             else {                                
//                 free(line_data->tokens);
//                 free(line_data->line);
//                 free(line_data);
//             }
//         }
//     }
// }

