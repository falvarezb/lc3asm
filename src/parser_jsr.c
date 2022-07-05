
#include "../include/lc3.h"

static bool strtolong2(char *str, long *num) {
    char *pEnd;
    long l = strtol(str, &pEnd, 10);
    if(str == pEnd) {
        return false;
    }
    else {
        *num = l;
        return true;
    }
}

/**
 * @brief
 *
 * Assembler formats:
 * JSR LABEL
 * JSR PCoffset11
 *
 * @param asm_instr JSR instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
uint16_t parse_jsr(char *asm_instr, uint16_t instruction_number) {

    //PARSING INSTRUCTION TOKENS
    long PCoffset11;
    char *instr_name = "JSR";
    int num_tokens = 2;
    char **tokens;

    if((tokens = instruction_tokens(asm_instr, instr_name, num_tokens)) == NULL) {        
        return do_return(0, tokens);
    }

    //VALIDATING TOKENS
    if(strcmp(tokens[0], instr_name)) {
        //this should not happen        
        printerr("expected %s but found %s\n", instr_name, tokens[0]);        
        return do_return(0, tokens);
    }

    //label or PCoffset11?
    if(!strtolong2(tokens[1], &PCoffset11)) {    
        //transform label into PCoffset11 by retrieving the memory location corresponding to the label from symbol table
        node_t *node = lookup(tokens[1]);
        if(!node) {
            printerr("Symbol not found ('%s')\n", tokens[1]);            
            return do_return(0, tokens);
        }
        PCoffset11 = node->val - instruction_number - 1;
    }
    else {
        //validate PCoffset11 numerical range
        if(PCoffset11 < -1024 || PCoffset11 > 1023) {
            printerr("value of PCoffset11 %ld is outside the range [-1024, 1023]\n", PCoffset11);            
            return do_return(0, tokens);
        }
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0100
    uint16_t machine_instr = 4 << 12;

    //bit[11]
    machine_instr += (1 << 11);

    //LABEL
    machine_instr += (PCoffset11);

    return do_return(machine_instr, tokens);
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
