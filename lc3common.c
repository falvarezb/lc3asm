#include "lc3common.h"

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

int is_imm5(char *token) {
    int imm5 = NO_IMM5_VALUE;
    char first_ch = *token;
    if(first_ch == '#') { //decimal literal
        imm5 = atoi(token + 1);
        return imm5 >= -16 && imm5 <= 15 ? imm5 : NO_IMM5_VALUE;
    }
    else if(first_ch == 'x') { //hex literal
        imm5 = sscanf(token + 1, "x");
        return imm5 >= -16 && imm5 <= 15 ? imm5 : NO_IMM5_VALUE;
    }
    else
        return imm5;
}

/**
 * @brief Truncate binary representation to the size of a LC3 word
 * 
 * @param decimal 
 * @return char* 
 */
char *bin_word(unsigned int decimal) {
    size_t i = CHAR_BIT * sizeof(int);
    return bin(decimal) + (i - LC3_WORD_SIZE);
}