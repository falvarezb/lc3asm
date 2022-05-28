#include "lc3common.h"
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

/*
    Parse the token representing the immediate operand and stores its numeric value in imm5
    imm5 is a 5-bit value, range [-16,15]
    imm5 can be expressed in decimal and hex notation

    Returns 0 if parsing is successful, else 1
    Error message is stored in errdesc
*/
int is_imm5(char *token, int *imm5) {
    char first_ch = *token;
    if(first_ch == '#') { //decimal literal
        *imm5 = atoi(token + 1);
        if(*imm5 < -16 || *imm5 > 15) {
            printerr("value of operand imm5 %s is outside the range [-16,15]\n", token + 1);
            return 1;
        }
        return 0;
    }
    else if(first_ch == 'x') { //hex literal
        if(sscanf(token + 1, "%x", imm5) < 1) {
            printerr("error while reading operand imm5 %s\n", token);
            return 1;
        }
        if(*imm5 < -16 || *imm5 > 15) {
            printerr("value of operand imm5 %s is outside the range [-16,15]\n", token + 1);
            return 1;
        }
        return 0;
    }
    printerr("operand imm5 %s must be decimal or hex\n", token);
    return 1;
}

/*
    Parse the token representing a memory location relative to the PC and stores its numeric value in PCoffset11
    PCoffset11 is a 11-bit value, range [-1024, 1023]

    Returns 0 if parsing is successful, else 1
    Error message is stored in errdesc
*/
int is_PCoffset11(char *token, long *PCoffset11) {    
    strtolong(token, &PCoffset11);

    if(PCoffset11 == NULL) {
        printerr("value of PCoffset11 %s is not a numeric value\n", token);
        return 1;
    }

    if(*PCoffset11 < 0 || *PCoffset11 > 1023) {
        printerr("value of PCoffset11 %s is outside the range [-1024, 1023]\n", token);
        return 1;
    }    

    return 0;
}


char **instruction_tokens(char *asm_instr, char *instr_name, int num_tokens) {
    char **tokens = (char **)malloc(num_tokens * sizeof(char *));
    if(tokens == NULL) {
        printerr("out of memory\n");
        return 0;
    }
    int i = 0;
    char *delimiters = " ,";
    char *pch = strtok(asm_instr, delimiters);
    while(pch != NULL) {
        if(i > num_tokens - 1) {
            printerr("unexpected token in %s instruction\n", instr_name);
            return NULL;
        }

        tokens[i++] = pch;
        pch = strtok(NULL, delimiters);
    }
    return tokens;
}

