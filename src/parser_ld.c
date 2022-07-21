/**
 * @file parser_jsr.c
 * @brief 
 * @version 0.1
 * @date 2022-07-11
 * 
 */

#include "../include/lc3.h"

/**
 * @brief Parse an assembly LD instruction and returns the corresponding machine instruction
 *
 * ## Assembly format
 * - LD DR,LABEL
 * - LD DR,PCoffset9
 * 
 * DR: Destination Register; one of R0..R7, which specifies which register the result of an instruction should be written to
 * 
 * PCoffset9: 9-bit value; bits [8:0] of an instruction; used with the PC+offset addressing mode. 
 * Bits [8:0] are taken as a 9-bit signed 2’s complement integer, sign-extended to 16 bits and then added to the incremented PC to form an address.
 * Range −256..255
 * 
 * LABEL: an assembly language construct that identifies a location symbolically (i.e., by means
 * of a name, rather than its 16-bit address).
 * In case of having a label, the corresponding PCoffset9 is worked out.
 *
 * @param operand LABEL or PCoffset9
 * @param instruction_counter instruction number in the assembly file
 * @param machine_instr 16-bit machine instruction (in case of error, it has undefined value)
 * @param line_counter line number of the assembly file
 * @return exit_t
 */
exit_t parse_ld(char *operand1, char *operand2, uint16_t instruction_counter, uint16_t *machine_instr, uint16_t line_counter) {
    
    int DR;
    long PCoffset9;  

    if((DR = is_register(operand1)) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_counter, operand1);
    }  

    //label or PCoffset9?
    if(!strtolong(operand2, &PCoffset9)) {    
        //transform label into PCoffset9 by retrieving the memory location corresponding to the label from symbol table
        node_t *node = lookup(operand2);
        if(!node) {
            return do_exit(EXIT_FAILURE,"ERROR (line %d): Symbol not found ('%s')", line_counter, operand2);                        
        }
        PCoffset9 = node->val - instruction_counter - 1;
    }
    else {
        //validate PCoffset9 numerical range
        if(PCoffset9 < -256 || PCoffset9 > 255) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Value of PCoffset9 %ld is outside the range [-256, 255]", line_counter, PCoffset9);                        
        }
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0010
    *machine_instr = 2 << 12;

    //DR
    DR = DR << 9;
    *machine_instr += DR;

    //LABEL
    *machine_instr += PCoffset9;

    return success();
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
