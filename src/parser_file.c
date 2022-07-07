/** @file */
#include "../include/lc3.h"
#include "../include/dict.h"

#define MAX_NUM_LABELS_PER_INSTRUCTION 10

static uint16_t parse_halt() {
    return 0xf025;
}

static uint16_t parse_orig() {    
    //FIXME proper implementation, hardcoded for now
    return 0x3000;
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
linetype_t compute_line_type(const char *line) {

    //saving line before it is modified by strtok    
    char *line_copy = strdup(line);

    char *delimiters = " ";
    char *assembly_instr = strtok(line_copy, delimiters);
    linetype_t result;
    if(assembly_instr == NULL || assembly_instr[0] == '\n') {
        result = BLANK_LINE;
    }
    else if(
        strcmp(assembly_instr, "ADD") == 0 ||
        strcmp(assembly_instr, "AND") == 0 ||
        strcmp(assembly_instr, "JMP") == 0 ||
        strcmp(assembly_instr, "JSR") == 0 ||
        strcmp(assembly_instr, "NOT") == 0 ||
        strcmp(assembly_instr, "RET") == 0 ||
        strcmp(assembly_instr, "HALT") == 0
        ) {
        result = OPCODE;
    }
    else if(strcmp(assembly_instr, ".ORIG") == 0) {
        result = ORIG_DIRECTIVE;
    }
    else if(strcmp(assembly_instr, ".END") == 0) {
        result = END_DIRECTIVE;
    }
    else if(assembly_instr[0] == ';') {
        result = COMMENT;
    }
    else {
        result = LABEL;
    }

    free(line_copy);
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
opcode_t compute_opcode_type(const char *line) {

    //saving line before it is modified by strtok    
    char *line_copy = strdup(line);

    char *delimiters = " ";
    char *assembly_instr = strtok(line_copy, delimiters);
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

    free(line_copy);
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
 * @return int 1 if there is an error, 0 otherwise (errdesc is set with the error details)
 */
int compute_symbol_table(FILE *source_file) {
    lineholder_t line_holder = { .whole_line = NULL, .partial_line = NULL };
    char *line = NULL;
    size_t len = 0;
    uint16_t instruction_counter = 0;
    int num_found_labels = 0; //number of found labels for a given instruction; reset to zero when labels are added to the symbol table
    char *found_labels[MAX_NUM_LABELS_PER_INSTRUCTION] = { 0 }; //array containing last found labels; reset to null when labels are added to the symbol table
    char *tmpline;

    errno = 0;
    ssize_t read = getline(&line_holder.whole_line, &len, source_file);
    while(read != -1) {
        //partial line takes precedence as it contains a subset of the whole line that needs to be processed independently
        if(line_holder.partial_line) {
            line = line_holder.partial_line;
        }
        else {
            line = line_holder.whole_line;
            line[read - 1] = '\0'; //remove newline char at the end of the line
        }
        printf("%s", line);

        linetype_t line_type = compute_line_type(line);

        if(line_type == END_DIRECTIVE) {
            for(int i = 0; i < num_found_labels; i++) {
                add(found_labels[i], instruction_counter);
                free(found_labels[i]);
            }
            //stop reading file
            break;
        }
        else if(line_type == ORIG_DIRECTIVE) {
            //read memory address of first instruction
            instruction_counter = parse_orig();
        }
        else if(line_type == COMMENT || line_type == BLANK_LINE) {
            //ignore line and continue            
        }
        else if(line_type == LABEL) {
            if(line_holder.partial_line) {
                //2 labels in the same line is disallowed
                printerr("invalid opcode ('%s')", line);
                for(int i = 0; i < num_found_labels; i++) {
                    free(found_labels[i]);
                }
                free(line_holder.whole_line);
                return EXIT_FAILURE;
            }
            tmpline = strdup(line);
            char *delimiters = " ";
            found_labels[num_found_labels++] = strtok(tmpline, delimiters);
            line_holder.partial_line = strtok(NULL, delimiters);
            if(line_holder.partial_line) {
                //instruction is in the same line as label                                
                read = strlen(line_holder.partial_line);
                continue;
            }
        }
        else if(line_type == OPCODE) {
            for(int i = 0; i < num_found_labels; i++) {
                add(found_labels[i], instruction_counter);
                free(found_labels[i]);
                found_labels[i] = NULL;
            }
            num_found_labels = 0;
            line_holder.partial_line = NULL;
            instruction_counter++;
        }
        else {
            //this should never happen as line_type takes values from an enum  
            printerr("unknown line type ('%d')", line_type);
            return EXIT_FAILURE;
        }

        read = getline(&line_holder.whole_line, &len, source_file);
    }

    free(line_holder.whole_line);

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
    uint16_t instruction_counter; // incremental value that represents the memory address of each instruction
    uint16_t machine_instr = 0; // 16-bit representation of machine instruction, 0 if assembly instruction cannot be parsed

    errno = 0;
    while((read = getline(&line, &len, source_file)) != -1) {
        printf("%s", line);
        //remove newline char at the end of the line
        line[read - 1] = '\0';

        linetype_t line_type = compute_line_type(line);

        if(line_type == END_DIRECTIVE) {
            //stop reading file
            break;
        }
        else if(line_type == COMMENT || line_type == BLANK_LINE) {
            //ignore line and continue
            continue;
        }
        else if(line_type == ORIG_DIRECTIVE) {
            machine_instr = parse_orig();
            instruction_counter = machine_instr;
            if(write_machine_instruction(machine_instr, destination_file)) {
                printerr("error writing instruction to object file\n");
                return EXIT_FAILURE;
            }
        }
        else if(line_type == OPCODE) {
            opcode_t opcode_type = compute_opcode_type(line);
            if(opcode_type == ADD) {
                machine_instr = parse_add(line);
            }
            else if(opcode_type == AND) {
                machine_instr = parse_and(line);
            }
            else if(opcode_type == JMP) {
                machine_instr = parse_jmp(line);
            }
            else if(opcode_type == JSR) {
                machine_instr = parse_jsr(line, instruction_counter);
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
                printerr("error writing instruction to object file\n");
                return EXIT_FAILURE;
            }
            instruction_counter++;
        }
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
