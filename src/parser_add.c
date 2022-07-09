
#include "../include/lc3.h"

/**
 * @brief
 *
 * Assembler formats:
 * ADD DR,SR1,SR2
 * ADD DR,SR1,imm5
 *
 * @param asm_instr ADD instruction
 * @return uint16_t* 16 bits representation of the instruction or 0 in case of error
 */
uint16_t parse_add(char *operand1, char* operand2, char* operand3) {

    //PARSING INSTRUCTION TOKENS
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
