
#include "../include/lc3.h"

/**
 * @brief Parse an assembly RET instruction and returns the corresponding machine instruction
 *
 * ## Assembly format
 * RET
 *
 * @param machine_instr 16-bit machine instruction (in case of error, it has undefined value)
 */
exit_t parse_ret(uint16_t *machine_instr) {

    //CONVERTING TO BINARY REPRESENTATION

    //instruction: 1100 000 111 000000
    *machine_instr = 49600;
    return success();  
}


#ifdef FAB_MAIN
int main(int argc, char const *argv[]) {

}
#endif
