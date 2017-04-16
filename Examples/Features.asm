.ORIG 0x3000            ; Each program needs to start with a .ORIG directive,
                        ; supplying it with an immediate value so that the
                        ; simulator (and the assembler) know what the beginning
                        ; address is.

.BLKW 0x10              ; Create 0x10 blocks of memory, all set to 0.
.BLKW 0x10 0xFF         ; Create 0x10 blocks of memory, initialised to 0xFF
cheat .BLKW b1 addr     ; Create a single block of memory initialised to the
                        ; address of the label 'addr'
.FILL 0x0               ; Set the address of this to 0.
.STRINGZ "Hello"        ; Create a null-terminated string.

; warning_check         ; Uncommenting this line should throw a warning about
                        ; multiple labels found for an address. This leads to
                        ; an error if BEGIN_TEST is used at any point.

; This is a label
BEGIN_TEST:
        AND R5, R5, #-10; Comments can be as close to the instruction as you'd
                        ; like
        ADD R1, R2, #1  // Comments can also be like so ('/' will is treated as
                        // a comment, but provides a warning)
        ADD R5, R5, 0

        LDI R7, cheat   ; As RET uses the value stored in R7, this will create
                        ; an infinite loop, unless any of the instructions
                        ; before this change the value at the address of label
                        ; 'addr' or at the address of label 'cheat'
        RET

addr    .FILL BEGIN_TEST; Put the address of the label 'BEGIN_TEST' into
                        ; memory at the address of the label 'addr'

b       .FILL 0x0       ; Put the value 0 into memory, and allow us to reference
                        ; this address by giving it a label.
bin     .FILL b1010     ; Binary literals are also supported.
hex     .FILL x0000     ; Hexadecimal too.

.END                    ; Each program needs to terminate with a .END directive.
                        ; If it's not found, then it just throws a warning.

