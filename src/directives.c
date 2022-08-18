/**
 * @file directives.c
 * @brief functions to generate machine code out of assembly directives
 * @version 0.1
 * @date 2022-07-12
 *
 */

#include "../include/lc3.h"

exit_t parse_orig(linemetadata_t *line_metadata) {    
    if(strcmp(line_metadata->tokens[0], ".ORIG")) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Instruction not preceeded by a .orig directive", line_metadata->line_number);
    }

    if(line_metadata->num_tokens < 2) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate expected", line_metadata->line_number);
    }

    long origin_address;
    exit_t result = parse_memory_address(line_metadata->tokens[1], &origin_address, line_metadata->line_number);
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
exit_t parse_fill(linemetadata_t *line_metadata, memaddr_t address_origin) {
    if(line_metadata->num_tokens < 2) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate expected", line_metadata->line_number);
    }

    char *token = line_metadata->tokens[1];
    char first_ch = *token;
    char *value_to_check;
    int base;
    if(first_ch == '#') { //decimal literal
        value_to_check = token + 1;
        base = 10;
    }
    else if(first_ch == 'x') { //hex literal
        value_to_check = token + 1;
        base = 16;
    }
    else { // decimal literal without prefix
        value_to_check = token;
        base = 10;
    }

    long offset;
    //is value a label or a number?
    if(!strtolong(value_to_check, &offset, base)) {
        //transform label into offset by retrieving the memory location corresponding to the label from symbol table
        node_t *node = lookup(token);
        if(!node) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Symbol not found ('%s')", line_metadata->line_number, token);
        }
        offset = node->val - 1 + address_origin;        
    }

    int16_t immediate = (int16_t) offset;
    line_metadata->machine_instruction = immediate;

    return success();
}

exit_t parse_blkw(linemetadata_t *line_metadata) {
    if(line_metadata->num_tokens < 2) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate expected", line_metadata->line_number);
    }

    long immediate;
    exit_t result = parse_memory_address(line_metadata->tokens[1], &immediate, line_metadata->line_number);
    if(result.code) {
        return result;
    }
    line_metadata->machine_instruction = immediate;

    return success();
}

exit_t parse_stringz(linemetadata_t *line_metadata) {
    if(line_metadata->num_tokens < 2) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate expected", line_metadata->line_number);
    }
    return success();   
}