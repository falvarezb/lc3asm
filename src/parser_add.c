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
 * @return uint16_t 16-bit machine instruction or 0 in case of error (errdesc is set with error details)
 */
exit_t parse_add(char *operand1, char* operand2, char* operand3, uint16_t *machine_instruction) {
    
    int DR, SR1, SR2;
    long imm5;    

    //VALIDATING OPERANDS

    if((DR = is_register(operand1)) == -1) {        
        return do_exit(EXIT_FAILURE, "expected register but found %s\n", operand1);        
    }

    if((SR1 = is_register(operand2)) == -1) {
        return do_exit(EXIT_FAILURE, "expected register but found %s\n", operand2);
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0001
    *machine_instruction = 1 << 12;

    //DR
    DR = DR << 9;
    *machine_instruction += DR;

    //SR1
    SR1 = SR1 << 6;
    *machine_instruction += SR1;

    if((SR2 = is_register(operand3)) > -1){
        *machine_instruction += SR2;
    }
    else {
        exit_t result = is_imm5(operand3, &imm5);
        if(result.code){
            return result;
        }
        *machine_instruction += (1 << 5);
        *machine_instruction += imm5;
    }

    return do_exit(EXIT_SUCCESS, NULL);    
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
