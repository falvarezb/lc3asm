
#include "../include/lc3.h"

/**
 * @brief Parse an assembly JMP instruction and returns the corresponding machine instruction
 *
 * ## Assembly format
 * - JMP BaseR
 * 
 * BaseR is a Base Register; one of R0..R7, used in conjunction with a six-bit offset to compute Base+offset addresses
 *
 * @param operand BaseR
 * @param machine_instr 16-bit machine instruction (in case of error, it has undefined value)
 * @param line_counter line number of the assembly file
 * @return exit_t
 */
exit_t parse_jmp(char *operand, uint16_t *machine_instr, uint16_t line_counter) {
    
    int BaseR;

    //VALIDATING OPERAND

    if((BaseR = is_register(operand)) == -1) {
        return do_exit(EXIT_FAILURE,"ERROR (line %d): Expected register but found %s", line_counter, operand);       
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 1100
    *machine_instr = 12 << 12;

    //BaseR
    BaseR = BaseR << 6;
    *machine_instr += BaseR;

    return do_exit(EXIT_SUCCESS, NULL);  
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
