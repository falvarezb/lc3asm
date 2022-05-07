#ifndef FAB_LC3
#define FAB_LC3
#include "lc3common.h"

/**
 * Notational conventions:
 *
 * SR: SR1, SR2: Source Register; one of R0..R7 which specifies the register from which a source operand is obtained
 *
 * DR: Destination Register; one of R0..R7, which specifies which register the result of an instruction should be written to
 *
 * imm5: A 5-bit immediate value; bits [4:0] of an instruction when used as a literal (immediate) value.
 * Taken as a 5-bit, 2’s complement integer, it is sign-extended to 16 bits before it is used. Range: −16..15
 *
 */

uint16_t parse_add(char *instr);
uint16_t parse_and(char *instr);

#endif
