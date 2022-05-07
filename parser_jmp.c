
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

    //READING INSTRUCTION TOKENS
    char *instr_name = "JMP";
    int BaseR;
    int num_tokens = 2;
    char *tokens[num_tokens];
    int i = 0;
    char *delimiters = " ,";
    char *pch = strtok(asm_instr, delimiters);
    while(pch != NULL) {
        if(i > num_tokens) {
            printerr("unexpected token in %s instruction\n", instr_name);
            return 0;
        }

        tokens[i++] = pch;
        pch = strtok(NULL, delimiters);
    }

    //PARSING TOKENS
    if(strcmp(tokens[0], instr_name)) {
        //this should not happen        
        printerr("expected JMP but found %s\n", tokens[0]);
        return 0;
    }

    if((BaseR = is_register(tokens[1])) == -1) {        
        printerr("expected register but found %s\n", tokens[1]);
        return 0;
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 1100
    uint16_t machine_instr = 12 << 12;

    //DR
    BaseR = BaseR << 6;
    machine_instr += BaseR;

    return machine_instr;
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
