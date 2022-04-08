;
;   Read three-digit decimal numbers from keyboard
;   Digit are read one at a time
;   Result is store in R1
;          
    .ORIG x3000
    LD R3,ASCII

    IN                  ; hundreds
    ADD R0,R0,R3
    LEA R1,LOOKUP100
    ADD R1,R1,R0
    LDR R1,R1,#0

    IN                  ; tens
    ADD R0,R0,R3
    LEA R2,LOOKUP10
    ADD R2,R2,R0
    LDR R2,R2,#0
    ADD R1,R1,R2

    IN                  ; units
    ADD R0,R0,R3
    ADD R1,R1,R0

    HALT

ASCII
    .FILL xFFD0     ;-x0030

LOOKUP100
    .FILL 0
    .FILL 100
    .FILL 200
    .FILL 300
    .FILL 400
    .FILL 500
    .FILL 600
    .FILL 700
    .FILL 800
    .FILL 900

LOOKUP10
    .FILL 0
    .FILL 10
    .FILL 20
    .FILL 30
    .FILL 40
    .FILL 50
    .FILL 60
    .FILL 70
    .FILL 80
    .FILL 90

    .END
    
