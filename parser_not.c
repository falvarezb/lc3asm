
#include "lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * NOT DR,SR
 *
 * @param asm_instr NOT instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
uint16_t parse_not(char *asm_instr) {

    //READING INSTRUCTION TOKENS
    int DR, SR;
    char *instr_name = "NOT";
    int num_tokens = 3;
    char **tokens;    

    if((tokens = instruction_tokens(asm_instr, instr_name, num_tokens)) == 0) {
        return 0;
    }

    //PARSING TOKENS
    if(strcmp(tokens[0], "NOT")) {
        //this should not happen        
        printerr("expected NOT but found %s\n", tokens[0]);
        return 0;
    }

    if((DR = is_register(tokens[1])) == -1) {        
        printerr("expected register but found %s\n", tokens[1]);
        return 0;
    }

    if((SR = is_register(tokens[2])) == -1) {
        printerr("expected register but found %s\n", tokens[2]);
        return 0;        
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 1001
    uint16_t machine_instr = 9 << 12;

    //DR
    DR = DR << 9;
    machine_instr += DR;

    //SR
    SR = SR << 6;
    machine_instr += SR;

    //last 6 bits = 111111
    machine_instr += 63;

    return machine_instr;
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
