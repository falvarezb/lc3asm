/**
 * @file assembler.c
 * @brief entry point to the application
 * @version 0.1
 * @date 2022-07-26
 *
 * Implementation notes
 * ====================
 *
 * The assembly process consists of two phases: [lexical analysis](https://en.wikipedia.org/wiki/Lexical_analysis) and [parsing](https://en.wikipedia.org/wiki/Parsing#:~:text=Parsing%2C%20syntax%20analysis%2C%20or%20syntactic,meaning%20part%20(of%20speech).).
 *
 * In the case of directives, parsing is done during the lexical analysis as directives need to be expanded into the corresponding instructions.
 * For instance, `.BLKW 2` is translated into two instructions to write `0`. This is necessary to ensure that the symbol table is correctly constructed.
 *
 * Ultimately, this is a consequence of instructions and data sharing the same address space and therefore being intermingled in memory.
 */

#include "../include/lc3.h"

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
exit_t serialize_symbol_table(FILE *destination_file, memaddr_t address_origin) {
    int num_chars_written;
    if((num_chars_written = fprintf(destination_file, "// Symbol table\n// Scope level 0:\n//	Symbol Name       Page Address\n//	----------------  ------------\n")) < 0) {
        return failure(EXIT_FAILURE, "error when writing serialized symbol table to file: %d", errno);
    }
    node_t *node = next(true);
    while(node) {
        memaddr_t label_address = node->val - 1 + address_origin;
        add(node->key, label_address);
        if((num_chars_written = fprintf(destination_file, "//	%s             %hx\n", node->key, label_address) < 0)) {
            return failure(EXIT_FAILURE, "error when writing serialized symbol table to file: %d", errno);
        }
        node = next(false);
    }
    return success();
}

static exit_t sym_obj_file_names(char *symbol_table_file_name, char *object_file_name, const char *assembly_file_name) {
    char *assemby_file_name_dup = strdup(assembly_file_name);
    char *file_extension = split_by_last_delimiter(assemby_file_name_dup, '.');
    if(strcmp(file_extension, "asm") != 0) {
        free(assemby_file_name_dup);
        return failure(EXIT_FAILURE, "ERROR: Input file must have .asm suffix ('%s')", assembly_file_name);
    }

    //.sym
    strcpy(symbol_table_file_name, assemby_file_name_dup);
    strcat(symbol_table_file_name, ".sym");

    //.obj
    strcpy(object_file_name, assemby_file_name_dup);
    strcat(object_file_name, ".obj");

    free(assemby_file_name_dup);
    return success();
}


exit_t assemble(const char *assembly_file_name) {
    //determine .sym and .obj file names
    char symbol_table_file_name[strlen(assembly_file_name) + strlen(".sym") + 1];
    char object_file_name[strlen(assembly_file_name) + strlen(".obj") + 1];
    exit_t result;

    if((result = sym_obj_file_names(symbol_table_file_name, object_file_name, assembly_file_name)).code) {
        return result;
    }

    //assembly file processing
    linemetadata_t **tokenized_lines = malloc(ADDRESS_SPACE_CARDINALITY * sizeof(linemetadata_t *));
    for(size_t i = 0; i < ADDRESS_SPACE_CARDINALITY; i++) {
        //setting sentinel values
        tokenized_lines[i] = NULL;
    }

    FILE *assembly_file = fopen(assembly_file_name, "r");
    if(!assembly_file) {
        return failure(EXIT_FAILURE, "ERROR: Couldn't read file (%s)", assembly_file_name);
    }

    result = do_lexical_analysis(assembly_file, tokenized_lines);
    fclose(assembly_file);
    if(result.code) {
        free_tokenized_lines(tokenized_lines);
        return result;
    }

    if((result = do_syntax_analysis(tokenized_lines)).code) {
        free_tokenized_lines(tokenized_lines);
        return result;
    }

    //symbol table update
    FILE *symbol_table_file = fopen(symbol_table_file_name, "w");
    if(!symbol_table_file) {
        free_tokenized_lines(tokenized_lines);
        return failure(EXIT_FAILURE, "ERROR: Couldn't open file (%s)", symbol_table_file_name);
    }

    result = serialize_symbol_table(symbol_table_file, tokenized_lines[0]->machine_instruction);
    fclose(symbol_table_file);
    if(result.code) {
        free_tokenized_lines(tokenized_lines);
        return result;
    }

    //object file
    FILE *object_file = fopen(object_file_name, "w");
    if(!object_file) {
        free_tokenized_lines(tokenized_lines);
        return failure(EXIT_FAILURE, "ERROR: Couldn't open file (%s)", object_file_name);
    }

    linemetadata_t *line_metadata;
    memaddr_t address_offset = 0;
    while((line_metadata = tokenized_lines[address_offset])) {
        if(write_machine_instruction(line_metadata->machine_instruction, object_file)) {
            fclose(object_file);
            return failure(EXIT_FAILURE, "ERROR: Couldn't write file (%s)", object_file_name);
        }
        free_line_metadata(line_metadata);
        address_offset++;
    }
    fclose(object_file);

    return success();
}

#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {
    exit_t result = assemble(argv[1]);
    if(result.code) {
        printf("\n\n==========================================\n");
        printf("%s\n", result.desc);
        printf("==========================================\n\n");
    }
    return result.code;
}
#endif


