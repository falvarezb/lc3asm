;
;   calculate n-th fibonacci number
;

    .ORIG x3000
    AND R0,R0,#0        ; R0 <- 0
    LD R1,N             ; R1 <- N
    AND R2,R2,#0        
    ADD R2,R2,#2        
    NOT R2,R2
    ADD R2,R2,#1
    ADD R1,R1,R2        ; R1 <- N - 2

    BRp NGT2            ; NGT2: N greater than 2
    ADD R0,R0,#1
    BR RESULT
        
NGT2    
    ADD R2,R0,#1        ; R2 <- 1                   
    ADD R3,R0,#1        ; R3 <- 1

LOOP    
    BRz RESULT
    ADD R0,R2,R3   ; R0 <- R2 + R3
    ADD R2,R3,#0   ; R2 <- R3
    ADD R3,R0,#0   ; R3 <- R0 
    ADD R1,R1,#-1   ; decrement loop variable
    BR LOOP

RESULT  
    STI R0,RESULT_DEST
    HALT


N               .FILL #7
RESULT_DEST     .FILL x3020

        .END