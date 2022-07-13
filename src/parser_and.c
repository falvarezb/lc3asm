
#include "../include/lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * AND DR,SR1,SR2
 * AND DR,SR1,imm5
 *
 * @param asm_instr AND instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
uint16_t parse_and(char *asm_instr) {

    //PARSING INSTRUCTION TOKENS
    int DR, SR1, SR2;
    long imm5;
    char *instr_name = "AND";
    int num_tokens = 4;
    char **tokens;    

    if((tokens = instruction_tokens(asm_instr, instr_name, num_tokens)) == NULL) {
        return do_return(0, tokens);
    }

    //VALIDATING TOKENS
    if(strcmp(tokens[0], "AND")) {
        //this should not happen        
        seterrdesc("expected AND but found %s\n", tokens[0]);
        return do_return(0, tokens);
    }

    if((DR = is_register(tokens[1])) == -1) {        
        seterrdesc("expected register but found %s\n", tokens[1]);
        return do_return(0, tokens);
    }

    if((SR1 = is_register(tokens[2])) == -1) {
        seterrdesc("expected register but found %s\n", tokens[2]);
        return do_return(0, tokens);        
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0101
    uint16_t machine_instr = 5 << 12;

    //DR
    DR = DR << 9;
    machine_instr += DR;

    //SR1
    SR1 = SR1 << 6;
    machine_instr += SR1;

    if((SR2 = is_register(tokens[3])) > -1){
        machine_instr += SR2;
    }
    else {
        if(is_imm5(tokens[3], &imm5)){
            return do_return(0, tokens);
        }
        machine_instr += (1 << 5);
        machine_instr += imm5;
    }

    return do_return(machine_instr, tokens);
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
