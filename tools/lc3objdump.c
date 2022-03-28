/*
    Prints the binary content of an object file generated by the LC3 assembler to stdout
    Each LC3 word (16 bits) is printed in a different line in the specified format (binary or hexadecimal)

    Example:

    franciscoalvarez@franciscos lc3asm % ./out/lc3objdump ../lc3practice/test.obj hex
    30 00
    20 02
    22 04
    0e 01
    00 01
    14 01
    f0 25
    30 0c

    franciscoalvarez@franciscos lc3asm % ./out/lc3objdump ../lc3practice/test.obj bin
    0011000000000000
    0010000000000010
    0010001000000100
    0000111000000001
    0000000000000001
    0001010000000001
    1111000000100101
    0011000000001100

*/

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#define BUF_SIZE 2
#define DEC_OUTPUT_MODE 0
#define BIN_OUTPUT_MODE 1
#define HEX_OUTPUT_MODE 2


void error_exit(const char *format, const char *text) {
    printf(format, errno, text);
    perror("");
    exit(EXIT_FAILURE);
}

int print_hex(unsigned char byte) {
    return printf("%.2x ", byte);
}

int print_bin(unsigned char byte) {
    int ret = 0;
    int i = CHAR_BIT;
    while(i--) {
        int printf_ret = 0;
        if((printf_ret = printf("%d", ((byte >> i) & 1))) < 0) {
            return printf_ret;
        }
        ret += printf_ret;
    }
    return ret;
}

int print_dec(unsigned char byte) {
    return printf("%.3d ", byte);
}

void read_file(char *filename, int output_mode) {

    //***** VARIABLES INITIALISATION
    FILE *inputFile;
    size_t numRead;
    // 0 <= buf[i] <= 255
    unsigned char buf[BUF_SIZE];

    int (*print_f) (unsigned char);
    if(output_mode == BIN_OUTPUT_MODE) {
        print_f = print_bin;
    }
    else if(output_mode == HEX_OUTPUT_MODE) {
        print_f = print_hex;
    }
    else {
        print_f = print_dec;
    }

    inputFile = fopen(filename, "rb"); //open binary file in read-only mode
    if(inputFile == NULL) {
        error_exit("error %d while opening file %s", filename);
    }

    //***** LOGIC TO READ FILE
    /* Read data until we encounter end of input or an error */
    while((numRead = fread(buf, 1, BUF_SIZE, inputFile)) > 0) {
        if(numRead < BUF_SIZE && !feof(inputFile)) {
            error_exit("error %d while reading file '%s'", filename);
        }

        for(size_t i = 0; i < numRead; i++) {
            if(print_f(buf[i]) < 0 && ferror(stdout)) {
                error_exit("error %d while writing '%s'", "");
            }
        }
        printf("\n");
    }

    if(ferror(inputFile)) {
        error_exit("error %d while reading file '%s'", filename);
    }

    if(fclose(inputFile) == EOF) perror("close input");
    exit(EXIT_SUCCESS);
}


#ifdef FAB_MAIN
int main(int argc, char *argv[]) {
    if(argc < 2  || (argc >= 3 && (strcmp(argv[2], "bin") != 0 && strcmp(argv[2], "hex") != 0))) {
        printf("USAGE %s filename [output_mode], output_mode=bin/hex\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int output_mode = DEC_OUTPUT_MODE;
    if(argc >= 3) {
        if(strcmp(argv[2], "bin") == 0) {
            output_mode = BIN_OUTPUT_MODE;
        }
        else {
            output_mode = HEX_OUTPUT_MODE;
        }
    }

    read_file(argv[1], output_mode);
}
#endif

