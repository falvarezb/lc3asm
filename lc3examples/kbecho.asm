
;
;   keyboard echo routine: 
;   char typed on the keyboard is printed on the screen
;
;   if we try to run this program, we'll get an IllegalMemAccessException
;   as the OS does not allow user programs to access device register
;   memory addresses. (I/O should be done through system calls by using
;   the TRAP mechanism)
;
;   we can get around this by changing the PSR register manually to set
;   the privilege mode (x8001)
;

        .ORIG x3000
POLL1   LDI R0,KBSRPtr  ; R0 points to register KBSR
        BRzp POLL1      ; keep polling until ready bit is set
        LDI R0,KBDRPtr  ; R0 points to register KBDR containing typed char

POLL2   LDI R1,DSRPtr   ; R1 points to register DSR
        BRzp POLL2      ; keep polling until ready bit is set
        STI R0,DDRPtr   ; typed char is stored in register DDR to be displayed

        HALT

KBSRPtr .FILL xFE00
KBDRPtr .FILL xFE02
DSRPtr  .FILL xFE04
DDRPtr  .FILL xFE06
        .END