
#include "../include/lc3.h"

/**
 * @brief Converts assembly 'NOT' instruction into machine instruction
 *
 * ### Assembly format
 * - NOT DR,SR
 *
 * SR: Source Register; one of R0..R7 which specifies the register from which a source operand is obtained
 * DR: Destination Register; one of R0..R7, which specifies which register the result of an instruction should be written to
 *
 * @param operand1 DR
 * @param operand2 SR
 * @param machine_instruction 16-bit machine instruction (in case of error, it has undefined value)
 * @param line_counter line number of the assembly file
 * @return exit_t
 */
exit_t parse_not(char *operand1, char *operand2, uint16_t *machine_instr, uint16_t line_counter) {

    int DR, SR;

    //VALIDATING OPERANDS

    if((DR = is_register(operand1)) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_counter, operand1);
    }

    if((SR = is_register(operand2)) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_counter, operand2);
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 1001
    *machine_instr = 9 << 12;

    //DR
    DR = DR << 9;
    *machine_instr += DR;

    //SR
    SR = SR << 6;
    *machine_instr += SR;

    //last 6 bits = 111111
    *machine_instr += 63;

    return success();
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
