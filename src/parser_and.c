
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
exit_t parse_and(char *asm_instr, uint16_t *machine_instr) {

    //PARSING INSTRUCTION TOKENS
    int DR, SR1, SR2;
    long imm5;
    char *instr_name = "AND";
    int num_tokens = 4;
    char **tokens;    

    if((tokens = instruction_tokens(asm_instr, instr_name, num_tokens)) == NULL) {
        return do_exit(EXIT_FAILURE, "unexpected token in AND instruction");
    }

    //VALIDATING TOKENS

    if((DR = is_register(tokens[1])) == -1) {        
        return do_exit(EXIT_FAILURE,"expected register but found %s\n", tokens[1]);        
    }

    if((SR1 = is_register(tokens[2])) == -1) {
        return do_exit(EXIT_FAILURE, "expected register but found %s\n", tokens[2]);
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0101
    *machine_instr = 5 << 12;

    //DR
    DR = DR << 9;
    *machine_instr += DR;

    //SR1
    SR1 = SR1 << 6;
    *machine_instr += SR1;

    if((SR2 = is_register(tokens[3])) > -1){
        *machine_instr += SR2;
    }
    else {
        exit_t result = is_imm5(tokens[3], &imm5);
        if(result.code){
            return result;
        }
        *machine_instr += (1 << 5);
        *machine_instr += imm5;
    }

    return do_exit(EXIT_SUCCESS, NULL);  
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
