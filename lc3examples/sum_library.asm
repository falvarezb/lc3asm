;
;   library to sum 2 values
;   expected params: R1,R2
;   return value: R3
;          
    .ORIG x3100

SUM
    ADD R3,R1,R2
    RET

    .END