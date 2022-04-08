        .ORIG x3000        
        LD R1,NUM       
        AND R1,R1,#1    ; even-odd test
        BRz EVEN        
        LEA R1,OUTPUT   ; "odd" branch
        LEA R2,ODDSTR          
        JMP R1
EVEN    LEA R2,EVENSTR  ; "even" branch
OUTPUT  LD R0,ASCII
        LD R1,NUM 
        ADD R0,R0,R1
        OUT
        ADD R0,R2,#0    ; R0 <- R2
        PUTS 
        HALT

NUM     .FILL #6        ; number to test
EVENSTR .STRINGZ " is even"
ODDSTR  .STRINGZ " is odd"
ASCII	.FILL	x0030
        .END