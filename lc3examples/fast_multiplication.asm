;
;   fast multiplication using shift-and-add algorithm
;   X*Y
;   IN: R0=X, R1=Y
;   OUT: R2=X*Y
;          
    .ORIG x3000

    LD R0,MULTIPLICAND
    LD R1,MULTIPLIER

    AND R2,R2,#0        ; initialise R2=0
    AND R3,R3,#0;       
    ADD R3,R3,#1        ; initialise R3=1
                        ; R3 is used to examine set bits in R1
    
    AND R4,R4,#0       
    ADD R4,R4,#15       ; initialise R4=15, loop counter
LOOP
    BRn END_LOOP
    AND R5,R1,R3
    BRz SHIFT_LEFT
    ADD R2,R2,R0
SHIFT_LEFT
    ADD R0,R0,R0        ; in binary, shift left equals multiplying by 2

    ADD R3,R3,R3        ; shifting bit 1 to the left
    ADD R4,R4,#-1       ; decrease counter
    BR LOOP

END_LOOP
    HALT

MULTIPLICAND    .FILL   #13
MULTIPLIER      .FILL   #3

    .END