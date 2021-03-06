; 
;   program that keeps prompting for an integer 'i' in the range 0-6, 
;   and each time it outputs the corresponding name of the day. 
;   If a key other than ’0’ through ’6’ is pressed, the program exits.
;

        .ORIG x3000
        GETC            ; R0 <- ASCII value of input char
        LD R1,ASCII
        NOT R1,R1       
        ADD R1,R1,#1
        ADD R1,R0,R1    ; convert ASCII to binary: R1 <- R0 - x0030

        ; the address of the corresponding day is DAYS + i*10
        LEA R0, DAYS
        ADD R1,R1,#0    ; to be able to use condition codes
LOOP    BRz OUTPUT
        ADD R0,R0,#10   ; go to next day
        ADD R1,R1,#-1   ; decrement loop variable
        BR LOOP

OUTPUT  PUTS
        HALT


ASCII	.FILL	x0030
DAYS    .STRINGZ    "Sunday   "
        .STRINGZ    "Monday   "
        .STRINGZ    "Tuesday  "
        .STRINGZ    "Wednesday"
        .STRINGZ    "Thursday "
        .STRINGZ    "Friday   "
        .STRINGZ    "Saturday "
    

        .END