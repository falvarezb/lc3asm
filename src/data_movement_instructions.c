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

    //VALIDATING OPERANDS

    if(line_metadata->num_tokens < 3) {
        return failure(EXIT_FAILURE, "ERROR (line %d): missing operands", line_metadata->line_number);
    }

    int SR_DR;

    if((SR_DR = parse_register(line_metadata->tokens[1])) == -1) {
        return failure(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[1]);
    }

    long offset;
    exit_t result = parse_offset(line_metadata->tokens[2], -256, 255, line_metadata->instruction_location, line_metadata->line_number, &offset, 9);
    if(result.code) {
        return result;
    }

    //CONVERTING TO BINARY REPRESENTATION

    uint16_t opcode_binary;
    switch(opcode) {
    case LD:
        //ops code: 0010
        opcode_binary = 2 << 12;
        break;
    case ST:
        //ops code: 0011
        opcode_binary = 3 << 12;
        break;
    case LDI:
        //ops code: 1010
        opcode_binary = 10 << 12;
        break;
    case STI:
        //ops code: 1011
        opcode_binary = 11 << 12;
        break;
    case LEA:
        //ops code: 1110
        opcode_binary = 14 << 12;
        break;
    default:
        assert(false);
        break;
    }
    line_metadata->machine_instruction = opcode_binary;

    //SR, DR
    line_metadata->machine_instruction += (SR_DR << 9);

    //LABEL
    line_metadata->machine_instruction += offset;

    return success();
}

exit_t parse_base_plus_offset_addressing_mode(linemetadata_t *line_metadata, opcode_t opcode) {
    
    //VALIDATING OPERANDS

    if(line_metadata->num_tokens < 4) {
        return failure(EXIT_FAILURE, "ERROR (line %d): missing operands", line_metadata->line_number);
    }

    int SR_DR, BaseR;

    if((SR_DR = parse_register(line_metadata->tokens[1])) == -1) {
        return failure(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[1]);
    }

    if((BaseR = parse_register(line_metadata->tokens[2])) == -1) {
        return failure(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[2]);
    }

    long offset;
    exit_t result = parse_offset(line_metadata->tokens[3], -32, 31, line_metadata->instruction_location, line_metadata->line_number, &offset, 6);
    if(result.code) {
        return result;
    }

    //CONVERTING TO BINARY REPRESENTATION

    uint16_t opcode_binary;
    switch(opcode) {
    case LDR:
        //ops code: 0110
        opcode_binary = 6 << 12;
        break;
    case STR:
        //ops code: 0111
        opcode_binary = 7 << 12;
        break;    
    default:
        assert(false);
        break;
    }
    line_metadata->machine_instruction = opcode_binary;

    //SR,DR    
    line_metadata->machine_instruction += (SR_DR << 9);

    //BaseR
    line_metadata->machine_instruction += (BaseR << 6);

    //offset6
    line_metadata->machine_instruction += offset;

    return success();
}

