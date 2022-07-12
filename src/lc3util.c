#include "../include/lc3util.h"
#define LC3_WORD_SIZE 16 // bits

/*
    Returns numeric value of the register or -1 if 'token' is not a register
*/
int is_register(char *token) {
    if(strcmp(token, "R0") == 0) {
        return 0;
    }
    else if(strcmp(token, "R1") == 0) {
        return 1;
    }
    else if(strcmp(token, "R2") == 0) {
        return 2;
    }
    else if(strcmp(token, "R3") == 0) {
        return 3;
    }
    else if(strcmp(token, "R4") == 0) {
        return 4;
    }
    else if(strcmp(token, "R5") == 0) {
        return 5;
    }
    else if(strcmp(token, "R6") == 0) {
        return 6;
    }
    else if(strcmp(token, "R7") == 0) {
        return 7;
    }
    return -1;
}

static int is_valid_immediate(char *token, long *imm, long min, long max) {
    char first_ch = *token;
    if(first_ch == '#') { //decimal literal
        if(!strtolong(token + 1, imm)) {
            printerr("immediate %s is not a numeric value\n", token);
            return EXIT_FAILURE;
        }
        if(*imm < min || *imm > max) {
            printerr("immediate %s is outside the range [%ld,%ld]\n", token + 1, min, max);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    else if(first_ch == 'x') { //hex literal
        if(sscanf(token + 1, "%lx", imm) < 1) {
            printerr("error while reading immediate %s\n", token);
            return EXIT_FAILURE;
        }
        if(*imm < min || *imm > max) {
            printerr("immediate %s is outside the range [%ld,%ld]\n", token + 1, min, max);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    printerr("immediate %s must be decimal or hex\n", token);
    return EXIT_FAILURE;
}

/*
    Parse the token representing the immediate operand and stores its numeric value in imm5
    imm5 is a 5-bit value, range [-16,15]
    imm5 can be expressed in decimal and hex notation

    Returns 0 if parsing is successful, else 1
    Error message is stored in errdesc
*/
int is_imm5(char *token, long *imm5) {
    return is_valid_immediate(token, imm5, -16, 15);
}

int is_valid_memaddr(char *token, long *memaddr) {
    return is_valid_immediate(token, memaddr, 0, 0xFFFF);
}

uint16_t do_return(uint16_t ret, char **tokens) {
    free(tokens);
    return ret;
}

char **instruction_tokens(char *asm_instr, char *instr_name, int num_tokens) {
    char **tokens = malloc(num_tokens * sizeof(char *));
    if(tokens == NULL) {
        printerr("out of memory\n");
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

            printerr("unexpected token in %s instruction\n", instr_name);
            free(tokens);
            return NULL;
        }

        tokens[i++] = pch;
        pch = strtok(NULL, delimiters);
    }
    return tokens;
}

