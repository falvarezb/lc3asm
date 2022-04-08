;
;   subroutine example
;   

    .ORIG x3000    
    
    LD R1,OPERAND1
    LD R2,OPERAND2
    LD R0,SUM
    JSRR R0
    HALT

OPERAND1    .FILL #1
OPERAND2    .FILL #2
SUM         .FILL x3100
    .END