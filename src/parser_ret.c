
#include "../include/lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * RET
 *
 * @param asm_instr RET instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
exit_t parse_ret(char *asm_instr, uint16_t *machine_instr) {

    //PARSING INSTRUCTION TOKENS    
    char *instr_name = "RET";
    int num_tokens = 1;
    char **tokens;    

    if((tokens = instruction_tokens(asm_instr, instr_name, num_tokens)) == NULL) {
        return do_exit(EXIT_FAILURE, "unexpected token in RET instruction");
    }

    //VALDATING TOKENS

    //CONVERTING TO BINARY REPRESENTATION

    //instruction: 1100 000 111 000000
    *machine_instr = 49600;
    return do_exit(EXIT_SUCCESS, NULL);  
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
