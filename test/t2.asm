;
;   template
;          
    .ORIG x3000
    JSR LABEL
    ADD R0,R0,#1
    HALT

LABEL
    ADD R0,R1,R2


    .END
