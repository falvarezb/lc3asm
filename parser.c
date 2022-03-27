
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

/**
 * Notational conventions:
 *
 * SR: SR1, SR2: Source Register; one of R0..R7 which specifies the register from which a source operand is obtained
 *
 * DR: Destination Register; one of R0..R7, which specifies which register the result of an instruction should be written to
 *
 * imm5: A 5-bit immediate value; bits [4:0] of an instruction when used as a literal (immediate) value.
 * Taken as a 5-bit, 2’s complement integer, it is sign-extended to 16 bits before it is used. Range: −16..15
 *
 */

void error_exit(const char *format, const char *text) {
    printf(format, text);
    exit(EXIT_FAILURE);
}

char *bin(unsigned int decimal) {
    size_t i = CHAR_BIT * sizeof(int);
    char *result = (char *)malloc(i);
    if(result == NULL) {
        error_exit("failure to allocate memory", "");
    }

    while(i--) {
        *(result + i) = ((decimal >> i) & 1);
    }
    return result;
}

int is_register(char *token) {
    if(strcmp(token, "R1") == 0) {
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
    int imm5 = 16;
    char first_ch = *token;
    if(first_ch == '#') { //decimal literal
        imm5 = atoi(first_ch + 1);
        return imm5 >= -16 && imm5 <= 15 ? imm5 : 16;
    }
    else if(first_ch == 'x') { //hex literal
        imm5 = sscanf(first_ch + 1, "x");
        return imm5 >= -16 && imm5 <= 15 ? imm5 : 16;
    }
    else
        return imm5;
}

/**
 * @brief
 *
 * Assembler formats:
 * ADD DR,SR1,SR2
 * ADD DR,SR1,imm5
 *
 * @param instr ADD instruction
 * @return char* binary representation
 */
char *parse_add(char *instr) {

    //READING INSTRUCTION TOKENS
    int DR;
    int SR1;
    int SR2;
    int imm5;
    char *tokens[4];
    int i = 0;
    char *delimiters = " ,";
    char *pch = strtok(instr, delimiters);
    while(pch != NULL) {
        if(i > 3) {
            error_exit("unexpected token in instruction %s", instr);
        }

        tokens[i++] = pch;
        pch = strtok(NULL, delimiters);
    }

    //VALIDATING TOKENS
    if(strcmp(tokens[0], "ADD")) {
        //this should not happen
        error_exit("expected ADD but found %s", tokens[0]);
    }

    if((DR = is_register(tokens[1])) == -1) {
        error_exit("expected register but found %s", tokens[1]);
    }

    if((SR1 = is_register(tokens[2])) == -1) {
        error_exit("expected register but found %s", tokens[2]);
    }

    if((SR2 = is_register(tokens[3])) == -1 || (imm5 = is_imm5(tokens[3]) == 16)) {
        error_exit("expected register or imm5 but found %s", tokens[3]);
    }


    //CONVERTING TO BINARY REPRESENTATION
    unsigned int ret;

    //ops code: 0001
    ret = 1 << 11;

    //DR
    DR = DR << 8;
    ret += DR;

    //SR1
    SR1 = SR1 << 5;
    ret += SR1;

    //imm5
    if(imm5 != 16) {
        ret += (1 << 4);
        ret += imm5;
    }
    else {
        ret += SR2;
    }

    return bin(ret);
}


char *parse_line(char *line) {

    return "";
}

#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif