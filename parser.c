
#include "lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * ADD DR,SR1,SR2
 * ADD DR,SR1,imm5
 *
 * @param instr ADD instruction
 * @return uint16_t* 16 bits representation of the instruction
 */
uint16_t *parse_add(char *instr) {

    //READING INSTRUCTION TOKENS
    int DR, SR1, SR2;
    int imm5 = NO_IMM5_VALUE;
    char *tokens[4];
    int i = 0;
    char *delimiters = " ,";
    char *pch = strtok(instr, delimiters);
    while(pch != NULL) {
        if(i > 3) {
            error_exit("unexpected token in instruction %s\n", instr);
        }

        tokens[i++] = pch;
        pch = strtok(NULL, delimiters);
    }

    //VALIDATING TOKENS
    if(strcmp(tokens[0], "ADD")) {
        //this should not happen
        error_exit("expected ADD but found %s\n", tokens[0]);
    }

    if((DR = is_register(tokens[1])) == -1) {
        error_exit("expected register but found %s\n", tokens[1]);
    }

    if((SR1 = is_register(tokens[2])) == -1) {
        error_exit("expected register but found %s\n", tokens[2]);
    }

    if((SR2 = is_register(tokens[3])) == -1 && (imm5 = is_imm5(tokens[3]) == NO_IMM5_VALUE)) {
        error_exit("expected register or imm5 but found %s\n", tokens[3]);
    }


    //CONVERTING TO BINARY REPRESENTATION
    uint16_t *ret = (uint16_t*) malloc(sizeof(u_int16_t));

    //ops code: 0001
    *ret = 1 << 12;

    //DR
    DR = DR << 9;
    *ret += DR;

    //SR1
    SR1 = SR1 << 6;
    *ret += SR1;

    //imm5
    if(imm5 != NO_IMM5_VALUE) {
        *ret += (1 << 5);
        *ret += imm5;
    }
    else {
        *ret += SR2;
    }

    return ret;
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
