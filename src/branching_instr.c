/**
 * @file branching_instr.c
 * @brief Contains functions to parse branching instructions
 * @version 0.1
 * @date 2022-07-11
 *
 */

#include "../include/lc3.h"

 /**
  * @brief Parse an assembly JSR instruction and returns the corresponding machine instruction
  *
  * ## Assembly format
  * - JSR LABEL
  * - JSR PCoffset11
  *
  * PCoffset11: 11-bit value; bits [10:0] of an instruction; used with the JSR opcode to compute the target address of a subroutine call.
  * Bits [10:0] are taken as an 11-bit 2’s complement integer, sign-extended to 16 bits and then added to the incremented PC to form the target address.
  * Range −1024..1023.
  *
  * LABEL: assembly language construct that identifies a location symbolically (i.e., by means
  * of a name, rather than its 16-bit address).
  * In case of having a label, the corresponding PCoffset11 is worked out.
  *
  * @param operand LABEL or PCoffset11
  * @param instruction_counter instruction number in the assembly file
  * @param machine_instr 16-bit machine instruction (in case of error, it has undefined value)
  * @param line_counter line number of the assembly file
  * @return exit_t
  */
exit_t parse_jsr(char *operand, uint16_t instruction_counter, uint16_t *machine_instr, uint16_t line_counter) {

    long offset;
    exit_t result = validate_offset(operand, -1024, 1023, instruction_counter, line_counter, &offset);
    if(result.code) {
        return result;
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0100
    *machine_instr = 4 << 12;

    //bit[11]
    *machine_instr += (1 << 11);

    //LABEL
    *machine_instr += offset;

    return success();
}

exit_t parse_br(char *operand, int condition_codes, uint16_t instruction_number, uint16_t *machine_instr, uint16_t line_counter) {

    long offset;
    exit_t result = validate_offset(operand, -256, 255, instruction_number, line_counter, &offset);
    if(result.code) {
        return result;
    }

    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0000
    *machine_instr = 0;

    //condition codes
    *machine_instr += (condition_codes << 9);

    //LABEL
    *machine_instr += offset;

    return success();
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
