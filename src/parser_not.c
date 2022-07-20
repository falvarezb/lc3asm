
#include "../include/lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * NOT DR,SR
 *
 * @param asm_instr NOT instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
exit_t parse_not(char *asm_instr, uint16_t *machine_instr) {

    //PARSING INSTRUCTION TOKENS
    int DR, SR;
    char *instr_name = "NOT";
    int num_tokens = 3;
    char **tokens;    

    if((tokens = instruction_tokens(asm_instr, instr_name, num_tokens)) == NULL) {
        return do_exit(EXIT_FAILURE, "unexpected token in NOT instruction");
    }

    //VALIDATING TOKENS

    if((DR = is_register(tokens[1])) == -1) {        
        return do_exit(EXIT_FAILURE,"expected register but found %s", tokens[1]);       
    }

    if((SR = is_register(tokens[2])) == -1) {
        return do_exit(EXIT_FAILURE,"expected register but found %s", tokens[2]);       
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 1001
    *machine_instr = 9 << 12;

    //DR
    DR = DR << 9;
    *machine_instr += DR;

    //SR
    SR = SR << 6;
    *machine_instr += SR;

    //last 6 bits = 111111
    *machine_instr += 63;

    return do_exit(EXIT_SUCCESS, NULL);  
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
