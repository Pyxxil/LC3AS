
.ORIG 0B0011000000000000; Each program needs to start with a .ORIG directive,
  ; supplying it with an immediate value so that the
  ; simulator (and the assembler) know what the beginning
  ; address is.
  .STRINGZ "jjs\""

  .BLKW 0x10              ; Create 0x10 blocks of memory, all set to 0.
  ; address of the label 'addr'
  .FILL 0x0               ; Set the address of this to 0.
  .STRINGZ "Hello"        ; Create a null-terminated string.

  ; warning_check         ; Uncommenting this line should throw a warning about
  ; multiple labels found for an address. This leads to
  ; an error if BEGIN_TEST is used at any point.

  ; This is a label
  BEGIN_TEST:
  ADD R5,, R5, ,#-10; Comments can be as close to the instruction as you'd
  ; like
  ADD R5,R5,R5
  ADD R1, R2, #1  // Comments can also be like so ('/' will is treated as
  // a comment, but provides a warning)
  ADD R5, R5, 0

  LDI R7, cheat   ; As RET uses the value stored in R7, this will create
  ; an infinite loop, unless any of the instructions
  ; before this change the value at the address of label
  ; 'addr' or at the address of label 'cheat'

  begin_test              ; Just to test the hash function
  .FILL 0

  EXTRA_FEATURES:         ; Some additional features not actually present in the
  ; LC3 (besides adding octal literals)
  .NEG R1         ; Negation
  .SUB R5, R1     ; Subtraction
  .SUB R5, R5     ; This is the exact same as AND R5, R5, #0 (it generates
      ; that exact instruction)
  .SUB R1, R1, R2
  .SUB R1, R2, R2
  .SUB R1, R2, R1
  .SUB R1, R2, R3

  oct     .FILL 0700     ; Octal values are also acceptable
  ;oct .FILL 0600 ; This should throw an error about multiple definitions
  ; of a label.
  .FILL 'A'
  .FILL '\n'

  .BLKW 0x10 0xFF ; Create 0x10 blocks of memory, initialised to 0xFF
  cheat   .BLKW b1 addr   ; Create a single block of memory initialised to the

  .SET R2, #10    ; This sets the register to the value on the right.
  .SET R3, 0x3000 ; As above

  .LSHIFT R2, #2

  ;JSR INCLUDED_LABEL      ; This should work, assuming addons are enabled
  ;.INCLUDE "include.h"

  JSR WARNING_THROWS
  WARNING_THROWS:         ; All of the following should throw warnings with
  ; --warn all
  JSR WARNING_THROWS ; Complains about an infinite loop
  JSR #0          ; Statement has no effect
  BRp #-1
  BRp #0          ; Statement has no effect
  BRn #-1         ; Complains about infinite loop
  BRn WARNING_THROWS ; Complains about same condition code as line before
  TRAP 0xFF       ; Possible illegal trap vector
  BRnzp #-256
  .LSHIFT R0, 1
  .LSHIFT R0, 2
  .LSHIFT R0, 3
  .LSHIFT R0, 4
  .LSHIFT R0, 5
  .LSHIFT R0, 6
  .LSHIFT R0, 7
  .LSHIFT R0, 8
  .LSHIFT R0, 9
  .LSHIFT R0, 10
  .LSHIFT R0, 11
  .LSHIFT R0, 12
  .LSHIFT R0, 13
  .LSHIFT R0, 14
  .LSHIFT R0, 15

  .NEG R1

  // .FILL -'\n'

  ; End of warnings
  RET
  Br WARNING_STRING        ; Should complain about the offset of 0
  WARNING_STRING:
  .STRINGZ "\ \ \ \ \ "    ; Should throw a warning about the lone '\'
  .STRINGZ "  " "Hello"           ; hello

  ; Uncomment the following for testing
  ;       Current Error Count = 38
  ;ERROR_THROWS:           ; These should all throw error's.
  ;        .STRINGZ 0x0
  ;        .STRINGZ addr
  ;        .STRINGZ R0
  ;        .STRINGZ "Non-terminated string
  ;        .FILL "Hello"
  ;        .FILL 'a
  ;        .FILL R0
  ;        .BLKW R0
  ;        .BLKW #1 R0
  ;
  ;        .BLKW -0x0
  ;        .FILL -b01
  ;        .FILL #A
  ;        .FILL #-b0
  ;        .FILL 0Ab
  ;
  ;        ADD R0, R0, addr
  ;        ADD R0, addr, R0
  ;        ADD addr, R0, R0
  ; ADD R0          ; This should only be an error for now, I'd like to
  ;                        ; implement this at some time.
  ADD R0, R0      ; As with above
  ;        ADD #0
  ;        ADD R0, #0      ; As with above
  ;        ADD R0, R0, R0, R0
  ;
  ;        AND R0, R0, addr
  ;        AND R0, addr, R0
  ;        AND addr, R0, R0
  ; AND R0          ; As with above
  AND R0, R0      ; As with above
  ;        AND #0
  ;        AND R0, #0      ; As with above
  ;        AND R0, R0, R0, R0
  ;
  ;        BR R0
  ;        BR "Hello"
  ;        BR add
  ;        BRz #-2000      ; Immediate value not representable in signed 9 bits
  ;        BR oc           ; Invalid label
  ;        .INCLUDE 0x1    ; Requires string
  ;        .LSHIFT R0, 0   ; Literal value too small
  ;        .LSHIFT R0, 16  ; Literal value too large

  BRnzp BEGIN_TEST

  addr    .FILL BEGIN_TEST; Put the address of the label 'BEGIN_TEST' into
  ; memory at the address of the label 'addr'

  b       .FILL 0x0       ; Put the value 0 into memory, and allow us to reference
  ; this address by giving it a label.
  bin     .FILL b1010     ; Binary literals are also supported.
  hex     .FILL x0000     ; Hexadecimal too.
  ;oct     .FILL \0000    ; Octal as well. This should throw an error about
  ; multiple definitions of a label

  .END                    ; Each program needs to terminate with a .END directive.
  ; If it's not found, then it just throws a warning.
  ;ADD R0, R0, R1         ; This should throw a warning.
  ;SOMETHING_THAT_WILL_BE_IGNORED
  ;        ADD R1, R2, R3

