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
 * @brief Parse the the .FILL directive
 *
 * `.FILL n` allocate one word and initialize with value n, where n can be:
 *  - an immediate value representing a memory address
 *  - a label representing a memory address
 *  - an immediate value representing an LC3 integer
 *
 *  Therefore, n can take values in the interval [-32768, 65535]
 *
 * @param line_metadata
 * @param address_origin if `n` is a label, then `address_origin` is needed to work out the final memory address
 * @return exit_t
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

    long numeric_value;
    //is value a label or a number?
    if(!strtolong(value_to_check, &numeric_value, base)) {
        node_t *node = lookup(token);
        if(!node) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Symbol not found ('%s')", line_metadata->line_number, token);
        }
        numeric_value = node->val - 1 + address_origin;
    }

    //validate numeric value range
    int min = -32768;
    int max = 65535;
    if(numeric_value < min || numeric_value > max) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Immediate operand (%s) out of range (%d to %d)", line_metadata->line_number, token, min, max);
    }

    int16_t immediate = (int16_t)numeric_value;
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

static exit_t interpret_escape_sequences(linemetadata_t *line_metadata) {
    char *token = line_metadata->tokens[1];
    bool escape_sequence = false;
    size_t i = 0, j = 0;
    char ch;
    while((ch = token[i]) != '\0') {
        if(escape_sequence) {
            switch(ch) {
            case 'a':
                token[j++] = '\a';
                escape_sequence = false;
                break;
            case 'b':
                token[j++] = '\b';
                escape_sequence = false;
                break;
            case 'e':
                token[j++] = '\e';
                escape_sequence = false;
                break;
            case 'f':
                token[j++] = '\f';
                escape_sequence = false;
                break;
            case 'n':
                token[j++] = '\n';
                escape_sequence = false;
                break;
            case 'r':
                token[j++] = '\r';
                escape_sequence = false;
                break;
            case 't':
                token[j++] = '\t';
                escape_sequence = false;
                break;
            case 'v':
                token[j++] = '\v';
                escape_sequence = false;
                break;
            case '\\':
                token[j++] = '\\';
                escape_sequence = false;
                break;
            default:
                //unrecognised escape sequence
                return do_exit(EXIT_FAILURE, "ERROR (line %d): Unrecognised escape sequence ('%s')", line_metadata->line_number, token);
                break;
            }
        }
        else if(ch == '\\') {
            escape_sequence = true;
        }
        else {
            token[j++] = ch;
        }
        i++;
    }
    if(j < i) {
        token[j++] = '\0';
    }
    return success();
}

exit_t parse_stringz(linemetadata_t *line_metadata, linemetadata_t *tokenized_lines[], memaddr_t *instruction_offset) {
    if(line_metadata->num_tokens < 2) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Bad string", line_metadata->line_number);
    }

    exit_t result = interpret_escape_sequences(line_metadata);
    if(result.code) {
        return result;
    }
    char *str_literal = line_metadata->tokens[1];
    for(size_t i = 0; i < strlen(str_literal); i++) {
        linemetadata_t *stringz_line_metadata = malloc(sizeof(linemetadata_t));
        if(!stringz_line_metadata) {
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Out of memory error", line_metadata->line_number);
        }
        stringz_line_metadata->tokens = NULL;
        stringz_line_metadata->line = NULL;
        stringz_line_metadata->machine_instruction = str_literal[i];
        tokenized_lines[*instruction_offset] = stringz_line_metadata;
        (*instruction_offset)++;
    }
    //final '\0'
    linemetadata_t *stringz_line_metadata = malloc(sizeof(linemetadata_t));
    if(!stringz_line_metadata) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Out of memory error", line_metadata->line_number);
    }
    stringz_line_metadata->tokens = NULL;
    stringz_line_metadata->machine_instruction = 0;
    tokenized_lines[*instruction_offset] = stringz_line_metadata;
    (*instruction_offset)++;

    return success();
}
