#include "../include/lc3.h"

uint16_t end_of_file() {
    printerr("END_OF_FILE");
    return 0;
}

uint16_t comment() {
    printerr("COMMENT");
    return 0;
}

uint16_t blank_line() {
    printerr("BLANK");
    return 0;
}

uint16_t parse_halt() {
    return 0xf025;
}

uint16_t parse_orig() {
    //FIXME proper implementation, hardcoded for now
    return 0x3000;
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
        printerr("out of memory");
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
        printerr("no instruction found in %s\n", line);
        result = 0;
    }

    free(line_copy);
    return result;
}

int parse_file(FILE *source_file, FILE *object_file) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    errno = 0;
    while((read = getline(&line, &len, source_file)) != -1) {                
        printf("%s", line);
        //remove newline char at the end of the line
        line[read-1] = '\0';

        uint16_t machine_instr = parse_line(line);

        if(machine_instr == 0) {
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
            fwrite(bytes, sizeof(char), 2, object_file);
        }
    }

    //check if getline resulted in error
    if(errno) {
        printerr("getLine error %d", errno);
        free(line);
        return EXIT_FAILURE;
    }

    free(line);
    return EXIT_SUCCESS;
}

#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {
    //parse_file();
}
#endif