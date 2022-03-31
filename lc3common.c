#include "lc3common.h"

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
    Returns 0 if parsing is successful, else 1
    Error message is stored in errdesc
*/
int is_imm5(char *token, int *imm5) {
    char first_ch = *token;
    if(first_ch == '#') { //decimal literal
        *imm5 = atoi(token + 1);
        if (*imm5 < -16 || *imm5 > 15) {
            snprintf(errdesc, ERR_DESC_LENGTH, "value of operand imm5 %s is outside the range [-16,15]", token);
            return 1;
        }
        return 0;
    }
    else if(first_ch == 'x') { //hex literal
        if(sscanf(token+1, "%x", imm5) < 1) {
            snprintf(errdesc, ERR_DESC_LENGTH, "error while reading operand imm5 %s", token);        
            return 1;
        }
        if (*imm5 < -16 || *imm5 > 15) {
            snprintf(errdesc, ERR_DESC_LENGTH, "value of operand imm5 %s is outside the range [-16,15]", token);
            return 1;
        }
        return 0;
    }
    snprintf(errdesc, ERR_DESC_LENGTH, "operand imm5 %s must be decimal or hex", token);
    return 1;
}

/**
 * @brief Binary representation truncated to the size of a LC3 word
 * 
 * @param decimal 
 * @return char* 
 */
char *bin_lc3word(unsigned int decimal) {
    size_t i = CHAR_BIT * sizeof(int);
    return bin(decimal) + (i - LC3_WORD_SIZE);
}
