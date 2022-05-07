
#include "lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * RET
 *
 * @param asm_instr RET instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
uint16_t parse_ret(char *asm_instr) {

    //READING INSTRUCTION TOKENS    
    char *tokens[1];
    int i = 0;
    char *delimiters = " ,";
    char *pch = strtok(asm_instr, delimiters);
    while(pch != NULL) {
        if(i > 1) {
            printerr("unexpected token in RET instruction\n", asm_instr);
            return 0;
        }

        tokens[i++] = pch;
        pch = strtok(NULL, delimiters);
    }

    //PARSING TOKENS
    if(strcmp(tokens[0], "RET")) {
        //this should not happen        
        printerr("expected RET but found %s\n", tokens[0]);
        return 0;
    }

    //CONVERTING TO BINARY REPRESENTATION

    //instruction: 1100 000 111 000000
    uint16_t machine_instr = 49600;
    return machine_instr;
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
