/**
 * @file parser_jsr.c
 * @brief 
 * @version 0.1
 * @date 2022-07-11
 * 
 */

#include "../include/lc3.h"

/**
 * @brief Parse an assembly JSR instruction and returns the corresponding machine instruction
 *
 * ## Assembler format
 * - JSR LABEL
 * - JSR PCoffset11
 * 
 * PCoffset11 is an 11-bit value; bits [10:0] of an instruction; used with the JSR opcode to compute the target address of a subroutine call. 
 * Bits [10:0] are taken as an 11-bit 2’s complement integer, sign-extended to 16 bits and then added to the incremented PC to form the target address. 
 * Range −1024..1023.
 * 
 * In case of having a label representing a memory address, the corresponding PCoffset11 is worked out.
 *
 * @param operand LABEL or PCoffset11
 * @param instruction_location memory address of the instruction being parsed
 * @return uint16_t 16-bit machine instruction or 0 in case of error (errdesc is set with error details)
 */
uint16_t parse_jsr(char *operand, uint16_t instruction_location) {
    
    long PCoffset11;    

    //label or PCoffset11?
    if(!strtolong(operand, &PCoffset11)) {    
        //transform label into PCoffset11 by retrieving the memory location corresponding to the label from symbol table
        node_t *node = lookup(operand);
        if(!node) {
            seterrdesc("Symbol not found ('%s')\n", operand);            
            return 0;
        }
        PCoffset11 = node->val - instruction_location - 1;
    }
    else {
        //validate PCoffset11 numerical range
        if(PCoffset11 < -1024 || PCoffset11 > 1023) {
            seterrdesc("value of PCoffset11 %ld is outside the range [-1024, 1023]\n", PCoffset11);            
            return 0;
        }
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0100
    uint16_t machine_instr = 4 << 12;

    //bit[11]
    machine_instr += (1 << 11);

    //LABEL
    machine_instr += PCoffset11;

    return machine_instr;
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
