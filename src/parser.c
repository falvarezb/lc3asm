
#include "../include/lc3.h"

exit_t do_syntax_analysis(linemetadata_t *tokenized_lines[]) {

    exit_t result;

    //1st instruction must be .ORIG
    linemetadata_t *line_metadata = tokenized_lines[0];
    if((result = parse_orig(line_metadata)).code) {
        return result;
    }
    //memaddr_t origin_address = line_metadata->machine_instruction; // incremental value that represents the memory address of each instruction    

    memaddr_t address_offset = 1;
    while((line_metadata = tokenized_lines[address_offset])) {
        linetype_t line_type = compute_line_type(line_metadata->tokens[0]);
        if(line_type == LABEL) {
            //two labels in the same line is disallowed 
            return do_exit(EXIT_FAILURE, "ERROR (line %d): Invalid opcode ('%s')", line_metadata->line_number, line_metadata->tokens[0]);
        }
        else if(line_type == FILL_DIRECTIVE) {
            result = parse_fill(line_metadata);
        }
        else if(line_type == OPCODE) {
            opcode_t opcode_type = compute_opcode_type(line_metadata->tokens[0]);
            switch(opcode_type) {
            case ADD: case AND:
                result = parse_binary_operation(line_metadata, opcode_type);
                break;
            case NOT:
                result = parse_not(line_metadata);
                break;
            case JSR:
                result = parse_jsr(line_metadata);
                break;
            case JMP:
                result = parse_jmp(line_metadata);
                break;
            case BR: case BRnzp:
                //condition codes: 111
                result = parse_br(line_metadata, 7);
                break;
            case BRn:
                //condition codes: 100
                result = parse_br(line_metadata, 4);
                break;
            case BRz:
                //condition codes: 010
                result = parse_br(line_metadata, 2);
                break;
            case BRp:
                //condition codes: 001
                result = parse_br(line_metadata, 1);
                break;
            case BRnz:
                //condition codes: 110
                result = parse_br(line_metadata, 6);
                break;
            case BRnp:
                //condition codes: 101
                result = parse_br(line_metadata, 5);
                break;
            case BRzp:
                //condition codes: 011
                result = parse_br(line_metadata, 3);
                break;
            case RET:
                //instruction: 1100 000 111 000000
                line_metadata->machine_instruction = 49600;
                result = success();
                break;
            case HALT:
                //instruction: 1100 000 111 000000
                line_metadata->machine_instruction = 0xf025;
                result = success();
                break;
            case LD: case ST: case LDI: case STI: case LEA:
                result = parse_pcoffset9_pattern(line_metadata, opcode_type);
                break;
            default:
                break;
            }
        }

        if(result.code) {
            return result;
        }        
        address_offset++;
    }

    //update symbol table?

    return result;

}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {
    //second_pass_parse();
}
#endif