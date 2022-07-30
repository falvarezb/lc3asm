/**
 * @file memory_access_instr.c
 * @brief Contains functions to parse instructions to read/write to memory
 * @version 0.1
 * @date 2022-07-11
 * 
 */

#include "../include/lc3.h"

/**
 * @brief Parse assembly instructions to access memory using a PC offset of 9 bits. Instructions in this category are: LD, ST, LDI, STI, LEA
 *
 * ## Assembly format examples
 * - LD DR,LABEL
 * - LD DR,PCoffset9
 * - ST SR,LABEL
 * - ST SR,PCoffset9
 * 
 * SR: Source Register; one of R0..R7 which specifies the register from which a source operand is obtained
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
 * @param operand1 source/destination register of the corresponding store/load instruction
 * @param operand2 LABEL or PCoffset9
 * @param instruction_counter instruction number in the assembly file
 * @param machine_instr 16-bit machine instruction (in case of error, it has undefined value)
 * @param line_counter line number of the assembly file
 * @param opcode opcode to identify whether it is a LD or STI instruction
 * @return exit_t
 */
exit_t parse_pc_relative_addressing_mode(linemetadata_t *line_metadata, opcode_t opcode) {

    if(line_metadata->num_tokens < 3) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): missing operands", line_metadata->line_number);
    }
    
    int lc3register;    

    if((lc3register = parse_register(line_metadata->tokens[1])) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[1]);
    }  

    long offset;
    exit_t result = parse_offset(line_metadata->tokens[2], -256, 255, line_metadata->instruction_location, line_metadata->line_number, &offset);
    if(result.code) {
        return result;
    }

    //CONVERTING TO BINARY REPRESENTATION

    uint16_t opcode_binary;
    if(opcode == LD) {
        //ops code: 0010
        opcode_binary = 2 << 12;
    }
    else if(opcode == ST) {
        //ops code: 0011
        opcode_binary = 3 << 12;
    }
    else if(opcode == LDI) {
        //ops code: 1010
        opcode_binary = 10 << 12;
    }
    else if(opcode == STI) {
        //ops code: 1011
        opcode_binary = 11 << 12;
    }
    else if(opcode == LEA) {
        //ops code: 1110
        opcode_binary = 14 << 12;
    }
    else {
        assert(false);
    }
    line_metadata->machine_instruction = opcode_binary;

    //DR
    lc3register = lc3register << 9;
    line_metadata->machine_instruction += lc3register;

    //LABEL
    line_metadata->machine_instruction += offset;

    return success();
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
