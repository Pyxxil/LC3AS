; A file containing a load of checks

.ORIG 0x3000

NO_WARNINGS_OR_ERRORS:
        ADD R0, R0, R0  ; Shouldn't warn or err
        AND R0, R0, R0  ; Shouldn't warn or err
.FILL   -\07
.INCLUDE "File that doesnt exist"
.FILL   #

WARNINGS:: :            ; Extraneous colons (warning count = 14)
.ORIG 0x3001
        ADD R0,, R0, R0 ; Extraneous comma
        AND R0, R0, ,R0 ; Extraneous comma
        NOT R4 , , R5   ; Extraneous comma
        , JMP R7        ; Extraneous comma
        JSR WARNINGS::  ; Extraneous colons
TEST:   BRn #0          ; Superfluous statement
        BRn #1          ; Checks same CC
        BRp #-1         ; Infinite loop

ERRORS: 9               ; Current error count (this line complains twice)
        ADD R0, R10, R0 ; Expected register
        AND R0, R8, R0  ; Invalid register
        JMP R9          ; Invalid register
        .STRINGZ "Here" "is a weird string"
        .FILL 0b0000101010101000101
        .FILL #99999
        .STRINGZ "Unterminated string

        JSR jdjdjjsjdsfkljsljfkjdsfkjlsdkjfksjdfkjsdkljf

.END
WARNING_LABEL
.END

