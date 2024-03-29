/**
 * @file branching_instr.c
 * @brief Contains functions to parse branching instructions
 * @version 0.1
 * @date 2022-07-11
 *
 */

#include "../include/lc3.h"


static exit_t parse_jump_instruction(linemetadata_t *line_metadata, opcode_t opcode) {
    assert(opcode == JSRR || opcode == JMP || opcode == JMPT);

    //VALIDATING OPERANDS
    int BaseR;

    if(line_metadata->num_tokens < 2) {
        return failure(EXIT_FAILURE, "ERROR (line %d): missing operands", line_metadata->line_number);
    }

    if((BaseR = parse_register(line_metadata->tokens[1])) == -1) {
        return failure(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[1]);
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0100
    line_metadata->machine_instruction = opcode == JSRR ? 4 << 12 : 12 << 12;

    //BaseR
    line_metadata->machine_instruction += (BaseR << 6);

    if(opcode == JMPT) {
        line_metadata->machine_instruction += 1;
    }

    return success();
}

 /**
  * @brief Parse an assembly JSR instruction and returns the corresponding machine instruction
  *
  * ## Assembly format
  * - JSR LABEL
  * - JSR PCoffset11
  *
  * PCoffset11: 11-bit value; bits [10:0] of an instruction; used with the JSR opcode to compute the target address of a subroutine call.
  * Bits [10:0] are taken as an 11-bit 2’s complement integer, sign-extended to 16 bits and then added to the incremented PC to form the target address.
  * Range −1024..1023.
  *
  * LABEL: assembly language construct that identifies a location symbolically (i.e., by means
  * of a name, rather than its 16-bit address).
  * In case of having a label, the corresponding PCoffset11 is worked out.
  *
  * @param operand LABEL or PCoffset11
  * @param instruction_counter instruction number in the assembly file
  * @param machine_instr 16-bit machine instruction (in case of error, it has undefined value)
  * @param line_counter line number of the assembly file
  * @return exit_t
  */
exit_t parse_jsr(linemetadata_t *line_metadata) {

    //VALIDATING OPERANDS

    if(line_metadata->num_tokens < 2) {
        return failure(EXIT_FAILURE, "ERROR (line %d): missing operands", line_metadata->line_number);
    }

    long offset;
    exit_t result = parse_offset(line_metadata->tokens[1], -1024, 1023, line_metadata->instruction_location, line_metadata->line_number, &offset, 11);
    if(result.code) {
        return result;
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0100
    line_metadata->machine_instruction = 4 << 12;

    //bit[11]
    line_metadata->machine_instruction += (1 << 11);

    //LABEL
    line_metadata->machine_instruction += offset;

    return success();
}

exit_t parse_jsrr(linemetadata_t *line_metadata) {
    return parse_jump_instruction(line_metadata, JSRR);
}

exit_t parse_br(linemetadata_t *line_metadata, int condition_codes) {

    //VALIDATING OPERANDS

    if(line_metadata->num_tokens < 2) {
        return failure(EXIT_FAILURE, "ERROR (line %d): missing operands", line_metadata->line_number);
    }

    long offset;
    exit_t result = parse_offset(line_metadata->tokens[1], -256, 255, line_metadata->instruction_location, line_metadata->line_number, &offset, 9);
    if(result.code) {
        return result;
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0000
    line_metadata->machine_instruction = 0;

    //condition codes
    line_metadata->machine_instruction += (condition_codes << 9);

    //LABEL
    line_metadata->machine_instruction += offset;

    return success();
}

/**
 * @brief Parse an assembly JMP instruction and returns the corresponding machine instruction
 *
 * ## Assembly format
 * - JMP BaseR
 * 
 * BaseR is a Base Register; one of R0..R7, used in conjunction with a six-bit offset to compute Base+offset addresses
 *
 * @param operand BaseR
 * @param machine_instr 16-bit machine instruction (in case of error, it has undefined value)
 * @param line_counter line number of the assembly file
 * @return exit_t
 */
exit_t parse_jmp(linemetadata_t *line_metadata) {
    return parse_jump_instruction(line_metadata, JMP);
}

exit_t parse_jmpt(linemetadata_t *line_metadata) {
    return parse_jump_instruction(line_metadata, JMPT);
}

exit_t parse_trap(linemetadata_t *line_metadata) {

    //VALIDATING OPERAND

    if(line_metadata->num_tokens < 2) {
        return failure(EXIT_FAILURE, "ERROR (line %d): missing operands", line_metadata->line_number);
    }
    
    long trapvector;    
    exit_t result = parse_trapvector(line_metadata->tokens[1], &trapvector, line_metadata->line_number);
    if(result.code) {
        return result;
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 1111
    line_metadata->machine_instruction = 15 << 12;

    //trapvector    
    line_metadata->machine_instruction += trapvector;

    return success();  
}

