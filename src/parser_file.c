/**
 * @file parser_file.c
 * @brief Parse content of assembly file (.asm) and generate the corresponding symbol table (.sym) and object file (.obj)
 * @version 0.1
 * @date 2022-07-08
 *
 * # Assembly syntax
 *
 * Each line in the assembly file can be:
 * - comment
 * - label (opcode operands) (comment)
 * - opcode operands (comment)
 *
 * Notes:
 * - opcodes also include pseudo-ops used for trap codes and assembler directives
 * - values within parentheses are optional
 *
 */

#include "../include/lc3.h"

#define MAX_NUM_LABELS_PER_INSTRUCTION 10

static void free_tokens(char **tokens, bool is_label_line) {
    if(is_label_line) {
        free(tokens - 1);
    }
    else {
        free(tokens);
    }
}

static int exit_compute_symbol_table(int result, char **tokens, bool is_label_line, char *line, FILE *file, const char *format, ...) {
    if(format) {
        va_list ap;
        va_start(ap, format);
        vsnprintf(errdesc, ERR_DESC_LENGTH, format, ap);
        va_end(ap);
    }
    free_tokens(tokens, is_label_line);
    free(line);
    fclose(file);
    return result;
}

static int exit_second_pass(int result, FILE *source_file, FILE *destination_file, char** tokens, const char* format, ...) {
    if(format) {
        va_list ap;
        va_start(ap, format);
        vsnprintf(errdesc, ERR_DESC_LENGTH, format, ap);
        va_end(ap);
    }
    fclose(source_file);
    fclose(destination_file);
    free(tokens);
    return EXIT_FAILURE;
}

static void add_labels_if_any_to_symbol_table(char *found_labels[], int *num_found_labels, uint16_t instruction_counter) {
    for(int i = 0; i < *num_found_labels; i++) {
        add(found_labels[i], instruction_counter);
        free(found_labels[i]);
        found_labels[i] = NULL;
    }
    *num_found_labels = 0;
}

static int write_machine_instruction(uint16_t machine_instr, FILE *destination_file) {
    char *bytes = (char *)&machine_instr;
    //swap bytes (because of little-endian representation)
    char byte = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = byte;
    size_t written = fwrite(bytes, sizeof(char), 2, destination_file);
    if(written != 2) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Serialize the symbol table as a string and writes it to the given file
 *
 * @param destination_file File where the symbol table is serialized
 * @return int 0 if serialization is successful or 1 if there is a writing error (errdesc is set with error details)
 */
int serialize_symbol_table(const char *symbol_table_file_name) {
    FILE *destination_file = fopen(symbol_table_file_name, "w");
    if(!destination_file) {
        seterrdesc("error when writing serialized symbol table to file");
        return EXIT_FAILURE;
    }
    int num_chars_written;
    if((num_chars_written = fprintf(destination_file, "// Symbol table\n// Scope level 0:\n//	Symbol Name       Page Address\n//	----------------  ------------\n")) < 0) {
        fclose(destination_file);
        seterrdesc("error when writing serialized symbol table to file");
        return EXIT_FAILURE;
    }
    node_t *node = next(true);
    while(node) {
        if((num_chars_written = fprintf(destination_file, "//	%s             %hx\n", node->key, node->val) < 0)) {
            fclose(destination_file);
            seterrdesc("error when writing serialized symbol table to file");
            return EXIT_FAILURE;
        }
        node = next(false);
    }
    fclose(destination_file);
    return EXIT_SUCCESS;
}

/**
 * @brief Determine the type of a line based on the value of the first token
 *
 * @param first_token first token of the line being parsed
 * @return linetype_t value indicative of the type of line
 */
linetype_t compute_line_type(const char *first_token) {

    linetype_t result;
    if(first_token[0] == '\n') {
        result = BLANK_LINE;
    }
    else if(
        strcmp(first_token, "ADD") == 0 ||
        strcmp(first_token, "AND") == 0 ||
        strcmp(first_token, "JMP") == 0 ||
        strcmp(first_token, "JSR") == 0 ||
        strcmp(first_token, "NOT") == 0 ||
        strcmp(first_token, "RET") == 0 ||
        strcmp(first_token, "HALT") == 0
        ) {
        result = OPCODE;
    }
    else if(strcmp(first_token, ".ORIG") == 0) {
        result = ORIG_DIRECTIVE;
    }
    else if(strcmp(first_token, ".END") == 0) {
        result = END_DIRECTIVE;
    }
    else if(first_token[0] == ';') {
        result = COMMENT;
    }
    else {
        result = LABEL;
    }

    return result;
}

/**
 * @brief Determine the opcode of an instruction
 *
 * @param opcode opcode to be analyzed
 * @return opcode_t value indicative of the type of opcode
 */
opcode_t compute_opcode_type(const char *opcode) {

    opcode_t result;
    if(strcmp(opcode, "ADD") == 0) {
        result = ADD;
    }
    else if(strcmp(opcode, "AND") == 0) {
        result = AND;
    }
    else if(strcmp(opcode, "JMP") == 0) {
        result = JMP;
    }
    else if(strcmp(opcode, "JSR") == 0) {
        result = JSR;
    }
    else if(strcmp(opcode, "NOT") == 0) {
        result = NOT;
    }
    else if(strcmp(opcode, "RET") == 0) {
        result = RET;
    }
    else {
        result = HALT;
    }
    return result;
}

/**
 * @brief Compute the symbol table by creating a dictionary to map labels to instructions.
 *
 * Each instruction is identified by its memory location.
 * The memory location of the first instruction is given by the directive `.ORIG`
 *
 * Each instruction in the asm file can be mapped up to __MAX_NUM_LABELS_PER_INSTRUCTION__ labels.
 *
 * Labels can be in the same line as the instruction or in a different one.
 * If in the same line, the label must come before the instruction.
 *
 * There cannot be more than one label in the same line.
 *
 * If there is an error, the function returns 1 and errdesc is updated with a description of the error.
 *
 * Possible error causes:
 * - unknown line type
 * - invalid opcode after a label
 * - error returned by _getline_
 *
 * @param source_file Source fle containing the asm code
 * @return int 1 if there is an error, 0 otherwise (errdesc is set with the error details). As a side effect, a file containing the symbol table is created
 */
int compute_symbol_table(const char *assembly_file_name) {
    //pointer to the line read; freed after finishing reading the file
    char *line = NULL;
    //array containing last found labels
    //since the array is defined on the stack, it does not need to be freed
    //however, found_labels[i] do as they are a copy of tokens[i]; freed when labels are added to the symbol table
    char *found_labels[MAX_NUM_LABELS_PER_INSTRUCTION] = { NULL };

    size_t len = 0;
    uint16_t line_counter = 0;
    uint16_t instruction_counter = 0;
    int num_found_labels = 0; //number of found labels for a given instruction, must be <= MAX_NUM_LABELS_PER_INSTRUCTION 
    bool orig_found = false;

    errno = 0;
    ssize_t read;
    FILE *source_file = fopen(assembly_file_name, "r");
    while((read = getline(&line, &len, source_file)) != -1) {
        printf("%s", line);
        line[read - 1] = '\0'; //remove newline char at the end of the line

        bool is_label_line = false;
        line_counter++;

        int num_tokens;
        //pointer to the array of tokens that the line is split into    
        //tokens[i] are pointers to different memory locations in `line` and do not need to be freed  
        //freed after processing the current line; 
        //when freeing, check if it's been advanced while processing a line with a label        
        char **tokens = split_tokens(line, &num_tokens, " ,");
        if(num_tokens == 0) {
            continue;
        }

        linetype_t line_type = compute_line_type(tokens[0]);
        if(line_type == LABEL) {
            found_labels[num_found_labels++] = strdup(tokens[0]);
            if(num_tokens > 1) {
                //continue processing the rest of the line as there are more elements after the label
                tokens++;
                is_label_line = true;
                line_type = compute_line_type(tokens[0]);
            }
            else {
                free(tokens);
                continue;
            }
        }

        if(line_type == END_DIRECTIVE) {
            add_labels_if_any_to_symbol_table(found_labels, &num_found_labels, instruction_counter);
            free_tokens(tokens, is_label_line);
            //stop reading file
            break;
        }
        else if(line_type == ORIG_DIRECTIVE) {
            //read memory address of first instruction
            if(num_tokens < 2) {
                return exit_compute_symbol_table(EXIT_FAILURE, tokens, is_label_line, line, source_file, "ERROR (line %d): Immediate expected", line_counter);
            }
            if((instruction_counter = orig(tokens[1])) == 0) {
                return exit_compute_symbol_table(EXIT_FAILURE, tokens, is_label_line, line, source_file, NULL);
            }
            orig_found = true;
        }
        else if(line_type == LABEL) {
            //two labels in the same line is disallowed            
            for(int i = 0; i < num_found_labels; i++) {
                free(found_labels[i]);
            }
            return exit_compute_symbol_table(EXIT_FAILURE, tokens, is_label_line, line, source_file, "invalid opcode ('%s')", tokens[0]);
        }
        else if(line_type == OPCODE) {
            add_labels_if_any_to_symbol_table(found_labels, &num_found_labels, instruction_counter);
            if(!orig_found) {
                return exit_compute_symbol_table(EXIT_FAILURE, tokens, is_label_line, line, source_file, "ERROR (line %d): Instruction not preceeded by a .orig directive", line_counter);
            }
            instruction_counter++;
        }
        free_tokens(tokens, is_label_line);

    }

    free(line);
    fclose(source_file);

    //check if getline resulted in error
    if(read == -1 && errno) {
        seterrdesc("getLine error %d\n", errno);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}



/**
 * @brief Executes first pass on the assembly source file
 *
 * During the first pass, lines of the source file are parsed to identify labels and create the symbol table.
 *
 * @param source_file Source file with assembly code
 * @param destination_file File containing the symbol table
 * @return int 0 if process is completed successfully, 1 otherwise (errdesc is set with error details)
 */
int first_pass_parse(const char *assembly_file_name, const char *symbol_table_file_name) {
    if(compute_symbol_table(assembly_file_name)) {
        return EXIT_FAILURE;
    }
    return serialize_symbol_table(symbol_table_file_name);
}

int second_pass_parse(const char *assembly_file_name, const char *object_file_name) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    uint16_t line_counter = 0;
    uint16_t instruction_counter = 0; // incremental value that represents the memory address of each instruction
    uint16_t machine_instr = 0; // 16-bit representation of machine instruction, 0 if assembly instruction cannot be parsed
    FILE *source_file = fopen(assembly_file_name, "r");
    FILE *destination_file = fopen(object_file_name, "w");

    errno = 0;
    while((read = getline(&line, &len, source_file)) != -1) {
        line_counter++;
        printf("%s", line);
        line[read - 1] = '\0'; //remove newline char at the end of the line
        int num_tokens;
        char **tokens = split_tokens(line, &num_tokens, " ,");
        if(num_tokens == 0) {
            continue;
        }

        linetype_t line_type = compute_line_type(tokens[0]);
        if(line_type == END_DIRECTIVE) {
            //stop reading file
            free(tokens);
            break;
        }
        else if(line_type == ORIG_DIRECTIVE) {
            machine_instr = orig(tokens[1]);
            instruction_counter = machine_instr;
            if(write_machine_instruction(machine_instr, destination_file)) {
                return exit_second_pass(EXIT_FAILURE, source_file, destination_file, tokens, "error writing instruction to object file");
            }
        }
        else if(line_type == OPCODE) {
            opcode_t opcode_type = compute_opcode_type(tokens[0]);
            if(opcode_type == ADD) {
                if(num_tokens < 4) {
                    return exit_second_pass(EXIT_FAILURE, source_file, destination_file, tokens, "ERROR (line %d): missing ADD operands", line_counter);
                }
                machine_instr = parse_add(tokens[1], tokens[2], tokens[3]);
            }
            else if(opcode_type == AND) {
                machine_instr = parse_and(line);
            }
            else if(opcode_type == JMP) {
                machine_instr = parse_jmp(line);
            }
            else if(opcode_type == JSR) {
                if(num_tokens < 2) {
                    return exit_second_pass(EXIT_FAILURE, source_file, destination_file, tokens, "ERROR (line %d): missing JSR operand", line_counter);
                }
                machine_instr = parse_jsr(tokens[1], instruction_counter);
            }
            else if(opcode_type == NOT) {
                machine_instr = parse_not(line);
            }
            else if(opcode_type == RET) {
                machine_instr = parse_ret(line);
            }
            else {
                machine_instr = halt();
            }
            if(write_machine_instruction(machine_instr, destination_file)) {
                return exit_second_pass(EXIT_FAILURE, source_file, destination_file, tokens, "error writing instruction to object file");
            }
            instruction_counter++;
        }
        free(tokens);
    }

    free(line);
    fclose(source_file);
    fclose(destination_file);

    //check if getline resulted in error
    if(read == -1 && errno) {
        seterrdesc("getLine error %d\n", errno);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int assemble(const char *assembly_file_name, const char *symbol_table_file_name, const char *object_file_name) {
    if(first_pass_parse(assembly_file_name, symbol_table_file_name)) {
        return EXIT_FAILURE;
    }

    return second_pass_parse(assembly_file_name, object_file_name);
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {
    //second_pass_parse();
}
#endif
