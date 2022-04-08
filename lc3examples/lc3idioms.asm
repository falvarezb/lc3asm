AND R3,R4,#0    ;Zero R3
ADD R4,R3,#0    ;Copy R3 to R4
ADD R3,R3,#1    ;Increment R3

NOT R4,R3
ADD R4,R4,#1    ;Copy -R3 to R4

ADD R4,R3,R3    ;Copy 2*R3 to R4