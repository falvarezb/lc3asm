
#include "../include/lc3.h"

/**
 * @brief Converts assembly 'AND' instruction into machine instruction
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
exit_t parse_and(char *operand1, char* operand2, char* operand3, uint16_t *machine_instr, uint16_t line_counter) {
    
    int DR, SR1, SR2;
    uint16_t imm5;

    //VALIDATING OPERANDS

    if((DR = is_register(operand1)) == -1) {        
        return do_exit(EXIT_FAILURE,"ERROR (line %d): Expected register but found %s", line_counter, operand1);        
    }

    if((SR1 = is_register(operand2)) == -1) {
        return do_exit(EXIT_FAILURE, "ERROR (line %d): Expected register but found %s", line_counter, operand2);
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0101
    *machine_instr = 5 << 12;

    //DR
    DR = DR << 9;
    *machine_instr += DR;

    //SR1
    SR1 = SR1 << 6;
    *machine_instr += SR1;

    if((SR2 = is_register(operand3)) > -1){
        *machine_instr += SR2;
    }
    else {
        exit_t result = is_imm5(operand3, &imm5, line_counter);
        if(result.code){
            return result;
        }
        *machine_instr += (1 << 5);
        *machine_instr += imm5;
    }

    return do_exit(EXIT_SUCCESS, NULL);  
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
