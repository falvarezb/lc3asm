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
 * @param operand1 DR
 * @param operand2 SR1
 * @param operand3 SR2 or imm5
 * @return uint16_t 16-bit machine instruction or 0 in case of error (errdesc is set with error details)
 */
uint16_t parse_add(char *operand1, char* operand2, char* operand3) {
    
    int DR, SR1, SR2;
    long imm5;    

    //VALIDATING OPERANDS

    if((DR = is_register(operand1)) == -1) {        
        printerr("expected register but found %s\n", operand1);
        return 0;
    }

    if((SR1 = is_register(operand2)) == -1) {
        printerr("expected register but found %s\n", operand2);
        return 0;       
    }


    //CONVERTING TO BINARY REPRESENTATION

    //ops code: 0001
    uint16_t machine_instr = 1 << 12;

    //DR
    DR = DR << 9;
    machine_instr += DR;

    //SR1
    SR1 = SR1 << 6;
    machine_instr += SR1;

    if((SR2 = is_register(operand3)) > -1){
        machine_instr += SR2;
    }
    else {
        if(is_imm5(operand3, &imm5)){
            return 0;
        }
        machine_instr += (1 << 5);
        machine_instr += imm5;
    }

    return machine_instr;    
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
