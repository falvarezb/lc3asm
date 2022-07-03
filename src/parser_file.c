/** @file */
#include "../include/lc3.h"
#include "../include/dict.h"

#define MAX_NUM_LABELS_PER_INSTRUCTION 10

static uint16_t end_of_file() {
    printerr("END_OF_FILE");
    return 0;
}

static uint16_t comment() {
    printerr("COMMENT");
    return 0;
}

static uint16_t label() {
    printerr("LABEL");
    return 0;
}

static uint16_t blank_line() {
    printerr("BLANK");
    return 0;
}

static uint16_t parse_halt() {
    return 0xf025;
}

static uint16_t parse_orig() {
    printerr(".ORIG");
    //FIXME proper implementation, hardcoded for now
    return 0x3000;
}

/**
 * @brief Serialize the symbol table as a string and writes it to the given file
 * 
 * @param destination_file File where the symbol table is serialized
 * @return int Returns 0 if serialization is successful or 1 if there is a writing error (errdesc is set accordingly)
 */
int serialize_symbol_table(FILE *destination_file) {
    int num_chars_written;
    if ((num_chars_written = fprintf(destination_file, "// Symbol table\n// Scope level 0:\n//	Symbol Name       Page Address\n//	----------------  ------------\n")) < 0) {
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
 * 
 * @param source_file Source fle containing the asm code
 * @return int Return code: 1 if there is an error, 0 otherwise
 */
int compute_symbol_table(FILE *source_file) {
    lineholder_t line_holder = { .whole_line = NULL, .partial_line = NULL };
    char *line = NULL;
    size_t len = 0;
    uint16_t instruction_counter = 0;
    int num_found_labels = 0;
    char *found_labels[MAX_NUM_LABELS_PER_INSTRUCTION] = {0}; //contains last found labels; reset to null when labels are added to the symbol table
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
        else if(
            line_type == ORIG_DIRECTIVE) {
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
    return EXIT_SUCCESS;
}



/**
 * @brief Executes first pass on the assembly source file
 * 
 * During the first pass, lines of the source file are parsed to identify labels and create the symbol table.
 *
 * @param source_file Source file with assembly code
 * @param destination_file The string resulting of serializing the symbol table is written to destination_file
 * @return int Return 0 if process is completed successfully, 1 otherwise (errdesc is set accordingly)
 */
int first_pass_parse(FILE *source_file, FILE *destination_file) {
    int result;
    if((result = compute_symbol_table(source_file))) {
        return result;
    }
    return serialize_symbol_table(destination_file);
}

/**
 * @brief parse line and return the binary representation of the instruction
 *
 * @param line line containing assembly instruction
 * @return uint16_t  16-bit representation of machine instruction, 0 if assembly instruction cannot be parsed
 */
uint16_t parse_line(char *line) {

    //saving line before it is modified by strtok    
    char *line_copy = strdup(line);
    if(line_copy == NULL) {
        printerr("out of memory\n");
        return 0;
    }

    char *delimiters = " ";
    char *assembly_instr = strtok(line_copy, delimiters);
    int result = 0;
    if(assembly_instr == NULL) {
        result = blank_line();
    }
    else if(strcmp(assembly_instr, "ADD") == 0) {
        result = parse_add(line);
    }
    else if(strcmp(assembly_instr, "AND") == 0) {
        result = parse_and(line);
    }
    else if(strcmp(assembly_instr, "JMP") == 0) {
        result = parse_jmp(line);
    }
    else if(strcmp(assembly_instr, "JSR") == 0) {
        result = parse_jsr(line);
    }
    else if(strcmp(assembly_instr, "NOT") == 0) {
        result = parse_not(line);
    }
    else if(strcmp(assembly_instr, "RET") == 0) {
        result = parse_ret(line);
    }
    else if(strcmp(assembly_instr, "HALT") == 0) {
        result = parse_halt();
    }
    else if(strcmp(assembly_instr, ".ORIG") == 0) {
        result = parse_orig();
    }
    else if(strcmp(assembly_instr, ".END") == 0) {
        result = end_of_file();
    }
    else if(assembly_instr[0] == ';') {
        result = comment();
    }
    else {
        result = label();
    }

    free(line_copy);
    return result;
}

int second_pass_parse(FILE *source_file, FILE *destination_file) {
    char *line = malloc(sizeof(char) * 1000);
    size_t len = 1000;
    ssize_t read;

    errno = 0;
    while((read = getline(&line, &len, source_file)) != -1) {
        printf("%s", line);
        //remove newline char at the end of the line
        line[read - 1] = '\0';

        uint16_t machine_instr = parse_line(line);

        if(!machine_instr) {
            if(strcmp(errdesc, "END_OF_FILE") == 0) {
                //stop reading file
                break;
            }
            else if(strcmp(errdesc, "COMMENT") == 0 || strcmp(errdesc, "BLANK") == 0) {
                //ignore line and continue
                clearerrdesc();
                continue;
            }
            else {
                free(line);
                return EXIT_FAILURE;
            }
        }
        else {
            char *bytes = (char *)&machine_instr;
            //swap bytes (because of little-endian representation)
            char byte = bytes[0];
            bytes[0] = bytes[1];
            bytes[1] = byte;
            fwrite(bytes, sizeof(char), 2, destination_file);
        }
    }

    free(line);

    //check if getline resulted in error
    if(errno) {
        printerr("getLine error %d\n", errno);
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {
    //second_pass_parse();
}
#endif
