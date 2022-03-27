
#include "lc3common.h"

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
    int DR, SR1, SR2;
    int imm5 = NO_IMM5_VALUE;
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

    if((SR2 = is_register(tokens[3])) == -1 || (imm5 = is_imm5(tokens[3]) == NO_IMM5_VALUE)) {
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
    if(imm5 != NO_IMM5_VALUE) {
        ret += (1 << 4);
        ret += imm5;
    }
    else {
        ret += SR2;
    }

    return bin(ret);
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
