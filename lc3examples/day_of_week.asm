;
;   calculate day of the week using Zeller's formula
;   f = k+(13m−1)/5+D+D/4+C/4−2C
;
;   if x is normal month number,
;   m=x+10 if x <= 2
;   m=x-2, otherwise
;
;   D is the last 2 digits of the year, but if it is January or February,
;   those of the previous year are used
;          
    .ORIG x3000

    LD R6,DAY_OF_THE_MONTH    ; R6 is f

    ; calculate m and D
    LD R1,YEAR          
    LD R2,HUNDRED
    JSR DIVISION        ; R1 is overwritten with value of D
                        ; R2 can be discarded

    LD R0,MONTH     ; calculate month m and store in R0
    ADD R2,R0,#-2   ; R2 can be discarded
    BRp MONTH_GT_2
    ADD R0,R0,#12
    ADD R1,R1,#-1   ; using previous year
    ST R1,D         ; R1 can be discarded
MONTH_GT_2
    ADD R0,R0,#-2

    ; calculate 13m
    AND R1,R1,#0
    ADD R1,R1,#13
    JSR MULTIPLICATION  ; R2 contains result of multiplication
                        ; R0 and R1 can be discarded
    
    ; calculate 13m-1
    ADD R2,R2,#-1

    ; calculate (13m-1)/5
    ADD R1,R2,#0        ; preparing R1 and R2 to call division
    AND R2,R2,#0
    ADD R2,R2,#5
    JSR DIVISION        ; R0 contains quotient
                        ; R1 and R2 can be discarded

    ADD R6,R6,R0        ; R0 can be discarded

    ; calculate D
    LD R1,D          
    ADD R6,R6,R1

    ; calculate D/4
    AND R2,R2,#0
    ADD R2,R2,#4
    JSR DIVISION        ; R1 is discarded
                        ; R0 contains quotient

    ADD R6,R6,R0        ; R0 can be discarded

    ; calculate C
    LD R1,YEAR          
    LD R2,HUNDRED
    JSR DIVISION        ; R0 is overwritten with value of C
                        ; R1 and R2 can be discarded
    ST R0,C

    ; calculate C/4
    ADD R1,R0,#0
    AND R2,R2,#0
    ADD R2,R2,#4
    JSR DIVISION        ; R0 is overwritten with value of quotient
                        ; R1 and R2 can be discarded

    ADD R6,R6,R0        ; R0 can be discarded

    ; calculate -2C
    LD R0,C
    ADD R0,R0,R0
    NOT R0,R0
    ADD R0,R0,#1

    ADD R6,R6,R0        ; R0 can be discarded

    ; f%7
    ADD R1,R6,#0
    AND R2,R2,#0
    ADD R2,R2,#7
    JSR DIVISION        ; R1 contains remainder
                        ; R0,R2 can be discarded

    ; print day of week
    JSR PRINT_WEEK_DAY
    HALT

MULTIPLICATION
    ST R3,R3_SAVE
    ST R4,R4_SAVE
    ST R5,R5_SAVE
    AND R2,R2,#0        ; initialise R2=0
    AND R3,R3,#0;       
    ADD R3,R3,#1        ; initialise R3=1
                        ; R3 is used to examine set bits in R1
    
    AND R4,R4,#0       
    ADD R4,R4,#15       ; initialise R4=15, loop counter
LOOP_MUL
    BRn END_LOOP_MUL
    AND R5,R1,R3
    BRz SHIFT_LEFT
    ADD R2,R2,R0
SHIFT_LEFT
    ADD R0,R0,R0        ; in binary, shift left equals multiplying by 2

    ADD R3,R3,R3        ; shifting bit 1 to the left
    ADD R4,R4,#-1       ; decrease counter
    BR LOOP_MUL

END_LOOP_MUL 
    LD R3,R3_SAVE
    LD R4,R4_SAVE
    LD R5,R5_SAVE
    RET

DIVISION
    ST R3,R3_SAVE
    AND R0,R0,#0        ; R0 is quotient    

    NOT R2,R2
    ADD R2,R2,#1        ; R2=-Y

LOOP_DIV
    ADD R3,R1,R2
    BRn END_LOOP_DIV
    ADD R0,R0,#1
    ADD R1,R3,#0
    BR LOOP_DIV

END_LOOP_DIV
    LD R3,R3_SAVE
    RET


PRINT_WEEK_DAY
    ; the address of the corresponding day is DAYS + i*10
    LEA R0, DAYS
    ADD R1,R1,#0    ; to be able to use condition codes
LOOP    
    BRz OUTPUT
    ADD R0,R0,#10   ; go to next day
    ADD R1,R1,#-1   ; decrement loop variable
    BR LOOP
OUTPUT  
    ST R7,R7_SAVE       ; saving return address as PUTS will overwrite it
    PUTS
    LD R7,R7_SAVE
    RET


DAY_OF_THE_MONTH    .FILL #6
MONTH               .FILL #3
YEAR                .FILL #2022
HUNDRED             .FILL #100
C                   .BLKW 1
D                   .BLKW 1
R3_SAVE             .BLKW 1
R4_SAVE             .BLKW 1
R5_SAVE             .BLKW 1
R7_SAVE             .BLKW 1

DAYS    
    .STRINGZ    "SUNDAY   "
    .STRINGZ    "MONDAY   "
    .STRINGZ    "TUESDAY  "
    .STRINGZ    "WEDNESDAY"
    .STRINGZ    "THURSDAY "
    .STRINGZ    "FRIDAY   "
    .STRINGZ    "SATURDAY "

    .END