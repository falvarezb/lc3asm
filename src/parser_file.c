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

int serialize_symbol_table(FILE *destination_file) {
    fprintf(destination_file, "// Symbol table\n// Scope level 0:\n//	Symbol Name       Page Address\n//	----------------  ------------\n");
    node_t *node = next(true);
    while(node) {
        fprintf(destination_file, "//	%s             %lx\n", node->key, node->val);
        node = next(false);
    }
    return EXIT_SUCCESS;
}

uint16_t parse_line_first_pass(const char *line, size_t *instruction_counter) {

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
        result = OPSCODE;
    }
    else if(strcmp(assembly_instr, ".ORIG") == 0) {
        result = ORIG_DIRECTIVE;
        *instruction_counter = parse_orig();
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

typedef struct {
    char *whole_line; //whole_line is handled by library function 'getline'
    char *partial_line; //next token after a label in the same line
} lineholder_t;

int compute_symbol_table(FILE *source_file) {
    lineholder_t line_holder = { NULL, NULL };
    char *line = NULL;
    size_t len = 0;
    size_t instruction_counter;
    //FIXME label should be an array of strings to allow several labels for the
    //same instruction
    //currently, only one label per instruction is supported
    int num_found_labels = 0;
    char *found_labels[MAX_NUM_LABELS_PER_INSTRUCTION] = {}; //contains last found labels; reset to null when label is added to the symbol table
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

        linetype_t line_type = parse_line_first_pass(line, &instruction_counter);

        if(line_type == END_DIRECTIVE) {
            for(int i = 0; i < num_found_labels; i++) {
                add(found_labels[i], instruction_counter);
                free(found_labels[i]);
            }
            //stop reading file
            break;
        }
        else if(
            line_type == COMMENT ||
            line_type == BLANK_LINE ||
            line_type == ORIG_DIRECTIVE
            ) {
            //ignore line and continue            
        }
        else if(line_type == LABEL) {
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
        else if(line_type == OPSCODE) {
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
            //error: unknown symbol            
            return EXIT_FAILURE;
        }

        read = getline(&line_holder.whole_line, &len, source_file);
    }

    free(line_holder.whole_line);
    return EXIT_SUCCESS;
}



/**
 * @brief parse line
 *
 * @param line line containing assembly instruction
 */
int first_pass(FILE *source_file, FILE *destination_file) {
    compute_symbol_table(source_file);
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

int parse_file(FILE *source_file, FILE *destination_file) {
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
    //parse_file();
}
#endif
