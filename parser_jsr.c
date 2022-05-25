
#include "lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * JSR LABEL
 *
 * @param asm_instr JSR instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
uint16_t parse_jsr(char *asm_instr) {

    //PARSING INSTRUCTION TOKENS
    int *LABEL = (int*)malloc(sizeof(int));
    char *instr_name = "JSR";
    int num_tokens = 2;
    char **tokens;    

    if((tokens = instruction_tokens(asm_instr, instr_name, num_tokens)) == 0) {
        return 0;
    }

    //VALIDATING TOKENS
    if(strcmp(tokens[0], instr_name)) {
        //this should not happen        
        printerr("expected %s but found %s\n", instr_name, tokens[0]);
        return 0;
    }

    if((is_PCoffset11(tokens[1], LABEL))) {        
        return 0;
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0100
    uint16_t machine_instr = 4 << 12;

    //bit[11]
    machine_instr += (1 << 11);

    //LABEL
    machine_instr += (*LABEL);

    return machine_instr;
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
