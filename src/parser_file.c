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
 *
 */

#include "../include/lc3.h"
#include "../include/dict.h"

#define MAX_NUM_LABELS_PER_INSTRUCTION 10

static uint16_t parse_halt() {
    return 0xf025;
}

static uint16_t parse_orig(char *token) {
    long memaddr;
    int result = is_valid_memaddr(token, &memaddr);
    if(result) {
        return 0;
    }
    return memaddr;
}

static void free_tokens(char **tokens, char *line_copy_ptr, bool is_label_line) {
    if(is_label_line) {
        free(tokens-1);
    }
    else {
    free(tokens);
    }
    //free(line_copy_ptr);
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
int serialize_symbol_table(FILE *destination_file) {
    int num_chars_written;
    if((num_chars_written = fprintf(destination_file, "// Symbol table\n// Scope level 0:\n//	Symbol Name       Page Address\n//	----------------  ------------\n")) < 0) {
        printerr("error when writing serialized symbol table to file");
        return EXIT_FAILURE;
    }
    node_t *node = next(true);
    while(node) {
        if((num_chars_written = fprintf(destination_file, "//	%s             %hx\n", node->key, node->val) < 0)) {
            printerr("error when writing serialized symbol table to file");
            return EXIT_FAILURE;
        }
        node = next(false);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Determine the type of the given line
 *
 * The possible types are determined by the values of enum linetype_t
 *
 * @param line line of the asm file to be inspected
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
 * @brief Determine the opcode type of the instruction in the given line
 *
 * The possible types are determined by the values of enum opcode_t
 *
 * @param opcode line of the asm file to be inspected
 * @return opcode_t value indicative of the type of opcode
 */
opcode_t compute_opcode_type(const char *assembly_instr) {

    opcode_t result;
    if(strcmp(assembly_instr, "ADD") == 0) {
        result = ADD;
    }
    else if(strcmp(assembly_instr, "AND") == 0) {
        result = AND;
    }
    else if(strcmp(assembly_instr, "JMP") == 0) {
        result = JMP;
    }
    else if(strcmp(assembly_instr, "JSR") == 0) {
        result = JSR;
    }
    else if(strcmp(assembly_instr, "NOT") == 0) {
        result = NOT;
    }
    else if(strcmp(assembly_instr, "RET") == 0) {
        result = RET;
    }
    else {
        result = HALT;
    }
    return result;
}


/**
 * @brief struct with information about the line being parsed
 *
 * At any given time, the line being parsed may be an entire line of the asm
 * file or the portion of a line that comes after a label:
 *
 * - whole_line is handled by the library function 'getline'
 * - partial_line is the next token after a label in the same line
 *
 */
typedef struct {
    char *whole_line; /*!< line of the file that is being parsed */
    char *partial_line; /*!< portion of a label line containing the next word after the label */
} lineholder_t;

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
int compute_symbol_table(FILE *source_file) {
    //malloc
    char *line = NULL; //must be freed before returning, it's updated on each line iteration
    char *line_copy_ptr; //must be freed at the end of each line iteration
    char **tokens; //must be freed at the end of each line iteration; tokens[i] are pointers to different memory locations in line and do not need to be freed

    //automatic
    size_t len = 0;
    uint16_t line_counter = 0;
    uint16_t instruction_counter = 0;
    int num_found_labels = 0; //number of found labels for a given instruction; reset to zero when labels are added to the symbol table

    //array containing last found labels; reset/freed when labels are added to the symbol table
    //found_labels[i] are a copy of tokens[i]
    char *found_labels[MAX_NUM_LABELS_PER_INSTRUCTION] = { NULL };
    bool is_label_line = false;

    errno = 0;
    ssize_t read;
    while((read = getline(&line, &len, source_file)) != -1) {
        is_label_line = false;
        line_counter++;
        printf("%s", line);
        line[read - 1] = '\0'; //remove newline char at the end of the line
        int num_tokens;
        tokens = line_tokens(line, &num_tokens, &line_copy_ptr);
        if(num_tokens == 0) {
            continue;
        }

        linetype_t line_type = compute_line_type(tokens[0]);
        if(line_type == LABEL) {
            found_labels[num_found_labels++] = strdup(tokens[0]);
            if(num_tokens > 1) {
                tokens = tokens + 1;
                is_label_line = true;
            }
            else {
                free_tokens(tokens, line_copy_ptr, is_label_line);
                continue;
            }
        }
        line_type = compute_line_type(tokens[0]);

        if(line_type == END_DIRECTIVE) {
            add_labels_if_any_to_symbol_table(found_labels, &num_found_labels, instruction_counter);
            free_tokens(tokens, line_copy_ptr, is_label_line);
            //stop reading file
            break;
        }
        else if(line_type == ORIG_DIRECTIVE) {
            //read memory address of first instruction
            if(num_tokens < 2) {
                printerr("ERROR (line %d): immediate expected", line_counter);
                free_tokens(tokens, line_copy_ptr, is_label_line);
                free(line);
                return EXIT_FAILURE;
            }
            instruction_counter = parse_orig(tokens[1]);
        }
        else if(line_type == LABEL) {
            //two labels in the same line is disallowed
            printerr("invalid opcode ('%s')", tokens[0]);
            for(int i = 0; i < num_found_labels; i++) {
                free(found_labels[i]);
            }
            free_tokens(tokens, line_copy_ptr, is_label_line);
            free(line);
            return EXIT_FAILURE;
        }
        else if(line_type == OPCODE) {
            add_labels_if_any_to_symbol_table(found_labels, &num_found_labels, instruction_counter);
            instruction_counter++;
        }
        free_tokens(tokens, line_copy_ptr, is_label_line);

    }

    free(line);

    //check if getline resulted in error
    if(read == -1 && errno) {
        printerr("getLine error %d\n", errno);
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
int first_pass_parse(FILE *source_file, FILE *destination_file) {
    if(compute_symbol_table(source_file)) {
        return EXIT_FAILURE;
    }
    return serialize_symbol_table(destination_file);
}

int second_pass_parse(FILE *source_file, FILE *destination_file) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    uint16_t line_counter = 0;
    uint16_t instruction_counter; // incremental value that represents the memory address of each instruction
    uint16_t machine_instr = 0; // 16-bit representation of machine instruction, 0 if assembly instruction cannot be parsed
    char *line_copy_ptr;
    errno = 0;
    while((read = getline(&line, &len, source_file)) != -1) {
        line_counter++;
        printf("%s", line);
        line[read - 1] = '\0'; //remove newline char at the end of the line
        int num_tokens;
        char **tokens = line_tokens(line, &num_tokens, &line_copy_ptr);
        if(num_tokens == 0) {
            continue;
        }

        linetype_t line_type = compute_line_type(tokens[0]);
        if(line_type == END_DIRECTIVE) {
            //stop reading file
            free_tokens(tokens, line_copy_ptr, false);
            break;
        }
        else if(line_type == COMMENT || line_type == BLANK_LINE) {
            //ignore line and continue
            free_tokens(tokens, line_copy_ptr, false);
            continue;
        }
        else if(line_type == ORIG_DIRECTIVE) {
            machine_instr = parse_orig(tokens[1]);
            instruction_counter = machine_instr;
            if(write_machine_instruction(machine_instr, destination_file)) {
                free_tokens(tokens, line_copy_ptr, false);
                printerr("error writing instruction to object file\n");
                return EXIT_FAILURE;
            }
        }
        else if(line_type == OPCODE) {
            opcode_t opcode_type = compute_opcode_type(tokens[0]);
            if(opcode_type == ADD) {
                if(num_tokens < 4) {
                    free_tokens(tokens, line_copy_ptr, false);
                    printerr("ERROR (line %d): missing ADD operands", line_counter);
                    return EXIT_FAILURE;
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
                    free_tokens(tokens, line_copy_ptr, false);
                    printerr("ERROR (line %d): missing JSR operand", line_counter);
                    return EXIT_FAILURE;
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
                machine_instr = parse_halt();
            }
            if(write_machine_instruction(machine_instr, destination_file)) {
                free_tokens(tokens, line_copy_ptr, false);
                printerr("error writing instruction to object file\n");
                return EXIT_FAILURE;
            }
            instruction_counter++;
        }
        free_tokens(tokens, line_copy_ptr, false);
    }

    free(line);

    //check if getline resulted in error
    if(read == -1 && errno) {
        printerr("getLine error %d\n", errno);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int assemble(FILE *source_file, FILE *symbol_table_file, FILE *object_file) {
    if(first_pass_parse(source_file, symbol_table_file)) {
        return EXIT_FAILURE;
    }
    rewind(source_file);
    return second_pass_parse(source_file, object_file);
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {
    //second_pass_parse();
}
#endif
