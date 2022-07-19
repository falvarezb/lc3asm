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

static int is_valid_immediate(char *token, long *imm, long min, long max) {
    char first_ch = *token;
    if(first_ch == '#') { //decimal literal
        if(!strtolong(token + 1, imm)) {
            seterrdesc("immediate %s is not a numeric value", token);
            return EXIT_FAILURE;
        }
        if(*imm < min || *imm > max) {
            seterrdesc("immediate operand (%s) outside of range (%ld to %ld)", token + 1, min, max);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    else if(first_ch == 'x') { //hex literal
        if(sscanf(token + 1, "%lx", imm) < 1) {
            seterrdesc("error while reading immediate %s", token);
            return EXIT_FAILURE;
        }
        if(*imm < min || *imm > max) {
            seterrdesc("immediate operand (%s) outside of range (%ld to %ld)", token + 1, min, max);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    seterrdesc("immediate %s must be decimal or hex", token);
    return EXIT_FAILURE;
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
int is_imm5(char *str, long *imm5) {
    return is_valid_immediate(str, imm5, -16, 15);
}

int is_valid_16bit_int(char *str, long *n) {
    return is_valid_immediate(str, n, 0, 0xFFFF);
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

