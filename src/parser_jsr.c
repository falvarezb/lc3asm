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
 * @param instruction_counter instruction number in the assembly file
 * @param machine_instruction 16-bit machine instruction (in case of error, it has undefined value)
 * @param line_counter line number of the assembly file
 * @return exit_t
 */
exit_t parse_jsr(char *operand, uint16_t instruction_counter, uint16_t *machine_instr, uint16_t line_counter) {
    
    long PCoffset11;    

    //label or PCoffset11?
    if(!strtolong(operand, &PCoffset11)) {    
        //transform label into PCoffset11 by retrieving the memory location corresponding to the label from symbol table
        node_t *node = lookup(operand);
        if(!node) {
            return do_exit(EXIT_FAILURE,"ERROR (line %d): Symbol not found ('%s')", line_counter, operand);                        
        }
        PCoffset11 = node->val - instruction_counter - 1;
    }
    else {
        //validate PCoffset11 numerical range
        if(PCoffset11 < -1024 || PCoffset11 > 1023) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Value of PCoffset11 %ld is outside the range [-1024, 1023]", line_counter, PCoffset11);                        
        }
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0100
    *machine_instr = 4 << 12;

    //bit[11]
    *machine_instr += (1 << 11);

    //LABEL
    *machine_instr += PCoffset11;

    return success();
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
