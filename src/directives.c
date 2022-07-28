/**
 * @file directives.c
 * @brief functions to generate machine code out of assembly directives
 * @version 0.1
 * @date 2022-07-12
 *
 */

#include "../include/lc3.h"

exit_t halt(uint16_t *machine_instr) {
    *machine_instr = 0xf025;
    return success();
}

/**
 * @brief Parse the operand of the .ORIG directive
 *
 * @param str operand representing starting address of the program
 * @return uint16_t numeric representation of the address or 0 if it cannot be parsed to a number
 */
exit_t orig(char *str, uint16_t *memaddr, uint16_t line_counter) {
    return is_valid_u16bit(str, memaddr, line_counter);
}

exit_t parse_orig(linemetadata_t *line_metadata) {    
    if(strcmp(line_metadata->tokens[0], ".ORIG")) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Instruction not preceeded by a .orig directive", line_metadata->line_number);
    }

    if(line_metadata->num_tokens < 2) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate expected", line_metadata->line_number);
    }

    memaddr_t origin_address;
    exit_t result = is_valid_u16bit(line_metadata->tokens[1], &origin_address, line_metadata->line_number);
    if(result.code) {
        return result;
    }
    line_metadata->machine_instruction = origin_address;

    return success();
}

/**
 * @brief Parse the operand of the .FILL directive
 *
 * `.FILL n` allocate one word and initialize with value n
 *
 * @param str operand representing the value to be allocated
 * @param n pointer to store the numeric representation of the operand
 * @return error_t
 */
// exit_t fill(char *str, uint16_t *n, uint16_t line_counter) {
//     return is_valid_16bit(str, n, line_counter);
// }

exit_t parse_fill(linemetadata_t *line_metadata) {
    if(line_metadata->num_tokens < 2) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate expected", line_metadata->line_number);
    }

    int16_t immediate;
    exit_t result = is_valid_lc3integer(line_metadata->tokens[1], &immediate, line_metadata->line_number);
    if(result.code) {
        return result;
    }
    line_metadata->machine_instruction = immediate;

    return success();
}