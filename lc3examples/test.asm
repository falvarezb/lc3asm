;
;   template
;          
    .ORIG x3000    
    LD  R0,A    
    LD  R1,B
    BR NEXT
    A   .FILL #1
    NEXT ADD R2,R0,R1
    HALT
    B   .FILL #12300
    .END

