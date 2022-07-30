/**
 * @file lexer.c
 * @brief Functionality to do the lexical analysis of the asm file
 * @version 0.1
 * @date 2022-07-23
 *
 */

#include "../include/lc3.h"

static void free_tokens(char **tokens, bool is_label_line) {
    if(is_label_line) {
        free(tokens - 1);
    }
    else {
        free(tokens);
    }
}

/**
 * @brief do the lexical analysis of the asm file
 *
 * Each line is analyzed separately: lines corresponding to instructions/directives are split into tokens and stored as
 * an element of the array `tokenized_lines`.
 * The symbol table is also created to store the offset of the instructions pointed to by the different labels found during the analysis.
 *
 * This function does not perform any syntax validation and as a consequence the lexer is not aware of the existence
 * or not of the .ORIG directive. That's why the resulting symbol table only stores offsets instead of the actual memory locations.
 *
 * Actual memory locations will be determined during syntax/semantic analysis by adding the previous offsets to the reference
 * memory address given by .ORIG.
 *
 * @param assembly_file handle to the asm file
 * @param tokenized_lines array to store line metadata generated by the lexer
 * @return exit_t
 */
exit_t do_lexical_analysis(FILE *assembly_file, linemetadata_t *tokenized_lines[]) {
    //pointer to the current line
    //on each iteration of the while loop, the value is overwritten with the contents of the current line    
    char *resusable_line = NULL;
    size_t len = 0; //length of the current line

    memaddr_t instruction_offset = 0; //real memory location = instruction offset + address given by .ORIG
    int line_counter = 0; //current line number in the assembly file

    errno = 0;
    ssize_t read;
    while((read = getline(&resusable_line, &len, assembly_file)) != -1) {
        printf("%s", resusable_line);
        bool is_label_line = false;
        line_counter++;

        char *line = strdup(resusable_line);
        int num_tokens = 0;
        char **tokens = split_tokens(line, &num_tokens, " ,\n");
        if(num_tokens == 0) {
            free(line);
            continue;
        }

        linetype_t line_type = compute_line_type(tokens[0]);
        if(line_type == LABEL) {
            add(tokens[0], instruction_offset);
            if(num_tokens > 1) {
                //continue processing the rest of the line as there are more elements after the label
                tokens++;
                num_tokens--;
                is_label_line = true;
                line_type = compute_line_type(tokens[0]);
            }
            else {
                free(line);
                free(tokens);
                continue;
            }
        }

        if(line_type == END_DIRECTIVE) {
            free(line);
            free_tokens(tokens, is_label_line);
            //stop reading file
            break;
        }
        else if(line_type == COMMENT || line_type == BLANK_LINE) {
            free(line);
            free_tokens(tokens, is_label_line);
            //ignore line
            continue;
        }

        linemetadata_t *line_metadata = malloc(sizeof(linemetadata_t));
        if(!line_metadata) {
            //out of memory error
        }
        line_metadata->tokens = tokens;
        line_metadata->num_tokens = num_tokens;
        line_metadata->is_label_line = is_label_line;
        line_metadata->line = line;
        line_metadata->line_number = line_counter;
        line_metadata->instruction_location = instruction_offset;
        tokenized_lines[instruction_offset] = line_metadata;

        if(line_type == BLKW_DIRECTIVE) {
            exit_t result = parse_blkw(line_metadata);
            if(result.code) {
                return result;
            }

            uint16_t blkw_operand = line_metadata->machine_instruction;
            for(size_t i = 0; i < blkw_operand; i++) {
                linemetadata_t *line_metadata = malloc(sizeof(linemetadata_t));
                if(!line_metadata) {
                    //out of memory error
                }
                line_metadata->tokens = NULL;
                line_metadata->line = NULL;
                line_metadata->machine_instruction = 0;
                tokenized_lines[instruction_offset] = line_metadata;
                instruction_offset++;
            }
            free(line);
            free_tokens(tokens, is_label_line);
        }
        else if(line_type == STRINGZ_DIRECTIVE) {
            exit_t result = parse_stringz(line_metadata);
            if(result.code) {
                return result;
            }

            char *str_literal = line_metadata->tokens[1];
            for(size_t i = 1; i < strlen(str_literal) - 1; i++) {
                //i=1...len-1 to omit quotation marks
                linemetadata_t *line_metadata = malloc(sizeof(linemetadata_t));
                if(!line_metadata) {
                    //out of memory error
                }
                line_metadata->tokens = NULL;
                line_metadata->line = NULL;
                line_metadata->machine_instruction = str_literal[i];
                tokenized_lines[instruction_offset] = line_metadata;
                instruction_offset++;
            }
            //final '\0'
            linemetadata_t *line_metadata = malloc(sizeof(linemetadata_t));
            if(!line_metadata) {
                //out of memory error
            }
            line_metadata->tokens = NULL;
            line_metadata->line = NULL;
            line_metadata->machine_instruction = 0;
            tokenized_lines[instruction_offset] = line_metadata;
            instruction_offset++;
            free(line);
            free_tokens(tokens, is_label_line);
        }
        else {
            instruction_offset++;
        }
    }

    free(resusable_line);
    //check if getline resulted in error
    if(read == -1 && errno) {
        return do_exit(EXIT_FAILURE, "getLine error %d\n", errno);
    }

    return success();
}
