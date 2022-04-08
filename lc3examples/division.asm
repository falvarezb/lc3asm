;
;   division : calculate quotient and remainder
;   X/Y = q*y + r
;   IN: R1=X, R2=Y
;   OUT: R0=q, R1=r
;          
    .ORIG x3000
    AND R0,R0,#0        ; R0 is quotient    
    LD R1,DIVIDEND      ; R1=X and in the end will contain remainder
    LD R2,DIVISOR       ; R2=Y

    NOT R2,R2
    ADD R2,R2,#1        ; R2=-Y

LOOP
    ADD R3,R1,R2
    BRn END_LOOP
    ADD R0,R0,#1
    ADD R1,R3,#0
    BR LOOP

END_LOOP
    HALT

DIVIDEND    .FILL   #13
DIVISOR     .FILL   #3

    .END