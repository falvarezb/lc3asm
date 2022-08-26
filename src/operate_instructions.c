/**
 * @file operate_instructions.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-07-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "../include/lc3.h"

exit_t parse_add_and(linemetadata_t *line_metadata, opcode_t opcode) {

    int DR, SR1, SR2;
    long imm5;

    //VALIDATING OPERANDS
    if(line_metadata->num_tokens < 4) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): missing operands", line_metadata->line_number);
    }

    if((DR = parse_register(line_metadata->tokens[1])) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[1]);
    }

    if((SR1 = parse_register(line_metadata->tokens[2])) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[2]);
    }


    //CONVERTING TO BINARY REPRESENTATION

    if(opcode == ADD) {
        //ops code: 0001
        line_metadata->machine_instruction = 1 << 12;
    }
    else if(opcode == AND) {
        //ops code: 0101
        line_metadata->machine_instruction = 5 << 12;
    }

    //DR    
    line_metadata->machine_instruction += (DR << 9);

    //SR1    
    line_metadata->machine_instruction += (SR1 << 6);

    if((SR2 = parse_register(line_metadata->tokens[3])) > -1) {
        line_metadata->machine_instruction += SR2;
    }
    else {
        exit_t result = parse_imm5(line_metadata->tokens[3], &imm5, line_metadata->line_number);
        if(result.code) {
            return result;
        }
        //5-bit 2's complement
        if(imm5 < 0) {
            imm5 += (1 << 5);
        }
        line_metadata->machine_instruction += (1 << 5);
        line_metadata->machine_instruction += imm5;
    }

    return success();
}

/**
 * @brief Converts assembly 'NOT' instruction into machine instruction
 *
 * ### Assembly format
 * - NOT DR,SR
 *
 * SR: Source Register; one of R0..R7 which specifies the register from which a source operand is obtained
 * DR: Destination Register; one of R0..R7, which specifies which register the result of an instruction should be written to
 *
 * @param operand1 DR
 * @param operand2 SR
 * @param machine_instr 16-bit machine instruction (in case of error, it has undefined value)
 * @param line_counter line number of the assembly file
 * @return exit_t
 */
exit_t parse_not(linemetadata_t *line_metadata) {

    int DR, SR;

    //VALIDATING OPERANDS
    if(line_metadata->num_tokens < 3) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): missing operands", line_metadata->line_number);
    }

    if((DR = parse_register(line_metadata->tokens[1])) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[1]);
    }

    if((SR = parse_register(line_metadata->tokens[2])) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[2]);
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 1001
    line_metadata->machine_instruction = 9 << 12;

    //DR
    DR = DR << 9;
    line_metadata->machine_instruction += DR;

    //SR
    SR = SR << 6;
    line_metadata->machine_instruction += SR;

    //last 6 bits = 111111
    line_metadata->machine_instruction += 63;

    return success();
}

