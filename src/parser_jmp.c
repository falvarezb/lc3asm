
#include "../include/lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * JMP BaseR
 *
 * @param asm_instr JMP instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
exit_t parse_jmp(char *asm_instr, uint16_t *machine_instr) {

    //PARSING INSTRUCTION TOKENS
    int BaseR;
    char *instr_name = "JMP";
    int num_tokens = 2;
    char **tokens;    

    if((tokens = instruction_tokens(asm_instr, instr_name, num_tokens)) == NULL) {
        return do_exit(EXIT_FAILURE, "unexpected token in JMP instruction");
    }

    //VALIDATING TOKENS

    if((BaseR = is_register(tokens[1])) == -1) {
        return do_exit(EXIT_FAILURE,"expected register but found %s", tokens[1]);       
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 1100
    *machine_instr = 12 << 12;

    //BaseR
    BaseR = BaseR << 6;
    *machine_instr += BaseR;

    return do_exit(EXIT_SUCCESS, NULL);  
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
