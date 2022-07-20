/**
 * @file lc3util.c
 * @brief functions common to the parsing of different instructions and directives
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

static exit_t is_valid_immediate(char *token, uint16_t *imm, long min, long max, uint16_t line_counter) {
    long tmp;
    char first_ch = *token;
    if(first_ch == '#') { //decimal literal
        if(!strtolong(token + 1, &tmp)) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate %s is not a numeric value", line_counter, token);
        }
        if(tmp < min || tmp > max) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate operand (%s) outside of range (%ld to %ld)", line_counter, token + 1, min, max);            
        }
        *imm = (uint16_t)tmp;
        return do_exit(EXIT_SUCCESS, NULL);
    }
    else if(first_ch == 'x') { //hex literal
        if(sscanf(token + 1, "%lx", &tmp) < 1) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Error while reading immediate %s", line_counter, token);            
        }
        if(tmp < min || tmp > max) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate operand (%s) outside of range (%ld to %ld)", line_counter, token + 1, min, max);            
        }
        *imm = (uint16_t)tmp;
        return do_exit(EXIT_SUCCESS, NULL);
    }
    return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate %s must be decimal or hex", line_counter, token);    
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
exit_t is_imm5(char *str, uint16_t *imm5, uint16_t line_counter) {
    return is_valid_immediate(str, imm5, -16, 15, line_counter);
}

exit_t is_valid_16bit_int(char *str, uint16_t *n, uint16_t line_counter) {
    return is_valid_immediate(str, n, 0, 0xFFFF, line_counter);
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

