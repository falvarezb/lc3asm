;
;   subroutine example
;          
    .ORIG x3000

    LD R1,OPERAND1
    LD R2,OPERAND2
    JSR SUM
    HALT

SUM                     ; subroutine to sum 2 values
    ADD R3,R1,R2
    RET

OPERAND1    .FILL #1
OPERAND2    .FILL #2
    .END