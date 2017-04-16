;
; These are, generally, the way that everything is handled. The assembler does
; not currently 100% satisfy the following, but it's being worked on.
;
; Immediate values:
; comment = (;|\/\/).*
; register = [rR][0-7]
; hexadecimal = 0?[xX]?[0-9a-fA-F]{0,4}
; decimal = #?-?\d+
; binary = 0?[bB]?[01]{0,16}
; octal = \[0-7]{1,7}
; string = ".*"
; label = In general, this is whatever isn't matched by anything else
; immediate = hexadecimal|decimal|octal|binary
;
; Note: For the following, all directives/instructions/trap routines are in
;       upper case, however they can be mixed case.
;
; Directives:
; orig    = \w*\.ORIG\w+immediate\w*comment?
; fill    = \w*\.FILL\w+(immediate|label)\w*comment?
; blkw    = \w*\.BLKW\w+immediate\w+(immediate|label)?\w*comment?
; stringz = \w*\.STRINGZ\w+string\w*comment?
; end     = \w*\.END\w*comment?
;
; directive = orig|fill|blkw|stringz|end
;
; Instructions:
; add  = \w*ADD\w+register\w*,?\w*register\w*,?\w*(register|immediate)\w*comment?
; and  = \w*AND\w+register\w*,?\w*register\w*,?\w*(register|immediate)\w*comment?
; br   = \w*BR[nN]?[zZ]?[pP]?\w+label\w*comment?  -- Do note that nzp can be in any order
; jmp  = \w*JMP\w+register\w*comment?
; jsr  = \w*JSR\w+label\w*comment
; jsrr = \w*JSRR\w+register\w*comment?
; ld   = \w*LD\w+register\w*,?\w*label\w*comment?
; ldi  = \w*LDI\w+register\w*,?\w*label\w*comment?
; ldr  = \w*LDR\w+register\w*,?\w*register\w*,?\w*immediate\w*comment?
; lea  = \w*LEA\w+register\w*,?\w*label\w*comment?
; not  = \w*NOT\w+register\w*,?\w*register\w*comment?
; ret  = \w*RET\w*comment?
; st   = \w*ST\w+register\w*,?\w*label\w*comment?
; sti  = \w*STI\w+register\w*,?\w*label\w*comment?
; str  = \w*STR\w+register\w*,?\w*register\w*,?\w*immediate\w*comment?
; trap = \w*TRAP\w+immediate\w*comment?
;
; instruction = add|and|br|jmp|jsr|jsrr|ld|ldi|ldr|lea|not|ret|st|sti|str|trap
;
; Trap routines:
; getc  = \w*GETC\w*comment?
; out   = \w*OUT\w*comment?
; puts  = \w*PUTS\w*comment?
; putsp =\w*PUTSP\w*comment?
; in    = \w*IN\w*comment?
; halt  = \w*HALT\w*comment?
;
; traproutine = getc|out|puts|putsp|in|halt
;
; This means that a valid line (one that isn't empty/just contains a comment is
; as such:
; (label:?)?(instruction|directive|traproutine)
;


.ORIG 0b0011000000000000; Each program needs to start with a .ORIG directive,
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

oct .FILL \700          ; Octal values are also acceptable

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
ADD R0, R0, R1          ; This should throw a warning.

