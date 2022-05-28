
#include "lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * JMP BaseR
 *
 * @param asm_instr JMP instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
uint16_t parse_jmp(char *asm_instr) {

    //PARSING INSTRUCTION TOKENS
    int BaseR;
    char *instr_name = "JMP";
    int num_tokens = 2;
    char **tokens;    

    if((tokens = instruction_tokens(asm_instr, instr_name, num_tokens)) == NULL) {
        return 0;
    }

    //VALIDATING TOKENS
    if(strcmp(tokens[0], instr_name)) {
        //this should not happen        
        printerr("expected %s but found %s\n", instr_name, tokens[0]);
        return 0;
    }

    if((BaseR = is_register(tokens[1])) == -1) {
        printerr("expected register but found %s\n", tokens[1]);
        return 0;
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 1100
    uint16_t machine_instr = 12 << 12;

    //BaseR
    BaseR = BaseR << 6;
    machine_instr += BaseR;

    return machine_instr;
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
