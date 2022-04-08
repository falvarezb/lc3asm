;
;   Display arbitrary-digit positive decimal numbers stored in R1
;   Digits are obtained by successively applying the mod 10 on the number and truncating until obtaining 0
;          
    .ORIG x3000
    LD R1,NUM
    LD R5,ASCII
    LD R6,STACK_BASE

    AND R2,R2,#0
    ADD R2,R2,#-10  ; R2=-10

LOOP
    ADD R4,R1,R2
    BRn END_LOOP
    JSR DIVISION
    ADD R0,R1,R5
    JSR PUSH
    ADD R1,R3,#0    ; quotient is the new dividend
    BR LOOP

END_LOOP
    ADD R0,R1,R5
    JSR PUSH

    ; printing digits from the stack
STACK_LOOP
    JSR ISEMPTY
    BRp END_STACK_LOOP
    JSR POP
    OUT
    BR STACK_LOOP

END_STACK_LOOP
    LD R0,NEWLINE
    OUT
    HALT

NUM     .FILL #123
ASCII   .FILL x0030
STACK_BASE .FILL x4000
MINUS_STACK_BASE .FILL xc000 ; -x4000
NEWLINE .FILL #10

DIVISION                ; R1=dividend and remainder, R2=divisor, R3=quotient
    AND R3,R3,#0           

LOOP_DIV
    ADD R4,R1,R2
    BRn END_LOOP_DIV
    ADD R3,R3,#1
    ADD R1,R4,#0
    BR LOOP_DIV

END_LOOP_DIV
    RET

PUSH
    STR R0,R6,#0
    ADD R6,R6,#1
    RET

POP
    ADD R6,R6,#-1
    LDR R0,R6,#0
    RET

ISEMPTY
    AND R0,R0,#0
    LD R5,MINUS_STACK_BASE
    ADD R1,R6,R5
    BRz EMPTY_STACK 
    ADD R0,R0,#0   
    BR END_ISEMPTY
EMPTY_STACK
    ADD R0,R0,#1
END_ISEMPTY
    RET

    .END
