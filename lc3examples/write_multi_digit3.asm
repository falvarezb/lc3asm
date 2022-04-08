;
;   Display three-digit positive decimal numbers stored in R1
;          
    .ORIG x3000
    LD R1,NUM
    LD R5,ASCII

    LD R4,HUNDRED
    AND R2,R2,#0        ; hundreds
    ADD R2,R2,R4
    JSR DIVISION
    ADD R0,R0,R5
    OUT

    AND R2,R2,#0        ; tens
    ADD R2,R2,#10
    JSR DIVISION
    ADD R0,R0,R5
    OUT

    ADD R0,R1,R5        ; units
    OUT

    HALT

NUM     .FILL #123
HUNDRED .FILL #100
ASCII   .FILL x0030     ;x0030

DIVISION
    AND R0,R0,#0        ; R0 is quotient    
    NOT R2,R2
    ADD R2,R2,#1        ; R2=-Y

LOOP
    ADD R3,R1,R2
    BRn END_LOOP
    ADD R0,R0,#1
    ADD R1,R3,#0
    BR LOOP

END_LOOP
    RET

    .END
    
