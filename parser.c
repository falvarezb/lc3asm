
#include "lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * ADD DR,SR1,SR2
 * ADD DR,SR1,imm5
 *
 * @param asm_instr ADD instruction
 * @return uint16_t* 16 bits representation of the instruction
 */
uint16_t *parse_add(char *asm_instr) {

    //READING INSTRUCTION TOKENS
    int DR, SR1, SR2;
    int imm5 = NO_IMM5_VALUE;
    char *tokens[4];
    int i = 0;
    char *delimiters = " ,";
    char *pch = strtok(asm_instr, delimiters);
    while(pch != NULL) {
        if(i > 3) {
            error_exit("unexpected token in instruction %s\n", asm_instr);
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

    if((SR2 = is_register(tokens[3])) == -1 && ((imm5 = is_imm5(tokens[3])) == NO_IMM5_VALUE)) {
        error_exit("expected register or imm5 but found %s\n", tokens[3]);
    }


    //CONVERTING TO BINARY REPRESENTATION
    uint16_t *machine_instr = (uint16_t*) malloc(sizeof(u_int16_t));

    //ops code: 0001
    *machine_instr = 1 << 12;

    //DR
    DR = DR << 9;
    *machine_instr += DR;

    //SR1
    SR1 = SR1 << 6;
    *machine_instr += SR1;

    //imm5
    if(imm5 != NO_IMM5_VALUE) {
        *machine_instr += (1 << 5);
        *machine_instr += imm5;
    }
    else {
        *machine_instr += SR2;
    }

    return machine_instr;
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
