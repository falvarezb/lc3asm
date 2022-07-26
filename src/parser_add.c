/**
 * @file parser_add.c
 * @brief
 * @version 0.1
 * @date 2022-07-11
 *
 */

#include "../include/lc3.h"

 /**
  * @brief Converts assembly 'ADD' instruction into machine instruction
  *
  * ### Assembly format
  * - ADD DR,SR1,SR2
  * - ADD DR,SR1,imm5
  *
  * SR1, SR2: Source Register; one of R0..R7 which specifies the register from which a source operand is obtained
  * DR: Destination Register; one of R0..R7, which specifies which register the result of an instruction should be written to
  *
  * imm5: A 5-bit immediate value; bits [4:0] of an instruction when used as a literal (immediate) value.
  * Taken as a 5-bit, 2’s complement integer, it is sign-extended to 16 bits before it is used. Range: −16..15
  *
  * @param operand1 DR
  * @param operand2 SR1
  * @param operand3 SR2 or imm5
  * @param machine_instr 16-bit machine instruction (in case of error, it has undefined value)
  * @param line_counter line number of the assembly file
  * @return exit_t
  */
exit_t parse_add(linemetadata_t *line_metadata) {

    int DR, SR1, SR2;
    uint16_t imm5;

    //VALIDATING OPERANDS
    if(line_metadata->num_tokens < 4) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): missing ADD operands", line_metadata->line_number);
    }

    if((DR = is_register(line_metadata->tokens[1])) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[1]);
    }

    if((SR1 = is_register(line_metadata->tokens[2])) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_metadata->line_number, line_metadata->tokens[2]);
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0001
    line_metadata->machine_instruction = 1 << 12;

    //DR
    DR = DR << 9;
    line_metadata->machine_instruction += DR;

    //SR1
    SR1 = SR1 << 6;
    line_metadata->machine_instruction += SR1;

    if((SR2 = is_register(line_metadata->tokens[3])) > -1) {
        line_metadata->machine_instruction += SR2;
    }
    else {
        exit_t result = is_imm5(line_metadata->tokens[3], &imm5, line_metadata->line_number);
        if(result.code) {
            return result;
        }
        line_metadata->machine_instruction += (1 << 5);
        line_metadata->machine_instruction += imm5;
    }

    return success();
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
