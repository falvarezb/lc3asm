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

static bool is_valid_string_literal(char *str) {
    char *first_quote = strchr(str, '"');
    char *last_quote = strrchr(str, '"');

    return first_quote == str && *last_quote == str[strlen(str)-1];
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

    if(is_valid_string_literal(line_metadata->tokens[1])) {
        return success();
    }
    else {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Bad string", line_metadata->line_number);
    }    
}