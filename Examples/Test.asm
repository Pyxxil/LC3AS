; A file containing a load of checks

.ORIG 0x3000

NO_WARNINGS_OR_ERRORS:
        ADD R0, R0, R0  ; Shouldn't warn or err
        AND R0, R0, R0  ; Shouldn't warn or err

WARNINGS:: :            ; Extraneous colons (warning count = 11)
        ADD R0,, R0, R0 ; Extraneous comma
        AND R0, R0, ,R0 ; Extraneous comma
        NOT R4 , , R5   ; Extraneous comma
        , JMP R7        ; Extraneous comma
        JSR WARNINGS::  ; Extraneous colons
        BRn #0          ; Superfluous statement
        BRn #1          ; Checks same CC
        BRp #-1         ; Infinite loop

ERRORS: 5               ; Current error count (this line complains twice)
        ADD R0, R10, R0 ; Expected register
        AND R0, R8, R0  ; Invalid register
        JMP R9          ; Invalid register

