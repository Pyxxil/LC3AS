;
; Add two 30-bit numbers, check whether or not they carried/overflowed, and print them to the display.
;


.ORIG x3000

        BR   START

OP1_LSW .FILL b101010101010101  ; The lowest 15 bits of the first operand.
OP1_MSW .FILL b110000000000010  ; The upper 15 bits of the first operand.
OP2_LSW .FILL b111111111111111  ; The lowest 15 bits of the second operand.
OP2_MSW .FILL b110000000000001  ; The upper 15 bits of the second operand.


; Start by printing the two operands
START:
        LD   R6, OP1_MSW
        JSR  BEGIN_PRINT_BINARY
        LD   R0, ASCII_SPACE
        OUT

        LD   R6, OP1_LSW
        JSR  BEGIN_PRINT_BINARY
        LD   R0, ASCII_NEWLINE
        OUT

        LD   R6, OP2_MSW
        JSR  BEGIN_PRINT_BINARY
        LD   R0, ASCII_SPACE
        OUT

        LD   R6, OP2_LSW
        JSR  BEGIN_PRINT_BINARY
        LD   R0, ASCII_NEWLINE
        OUT

        LEA  R0, SEPERATOR
        PUTS

        LD   R0, ASCII_NEWLINE
        OUT

; Add the least significant words of the two operands.
ADD_LSW:
        AND  R4, R4, #0
        LD   R0, OP1_LSW
        LD   R1, OP2_LSW
        ADD  R6, R0, R1
        LD   R5, MASK           ; Make sure the result is only 15 bits wide.
        AND  R5, R5, R6
        ST   R5, LSW_RESULT
        LD   R5, CARRY_BIT      ; Check whether or not the addition carried.
        AND  R5, R5, R6
        BRz  ADD_MSW            ; If it didn't carry, we don't need to add 1 to the MSW addition.

LSW_CARRY:
        ADD  R4, R4, #1

; Add the most significant words of the two operands.
ADD_MSW:
        LD   R0, OP1_MSW
        LD   R1, OP2_MSW
        ADD  R6, R1, R0
        ADD  R6, R6, R4
        LD   R5, MASK
        AND  R5, R5, R6
        ST   R5, MSW_RESULT
        LD   R5, CARRY_BIT      ; Check if the result carried.
        AND  R5, R5, R6
        BRz  CHECK_OVERFLOW     ; If it didn't carry, just start checking for overflow.

MSW_CARRY:
        ADD  R0, R0, #1         ; Store the fact that the addition did carry.
        ST   R0, CARRY

CHECK_OVERFLOW:
        LD   R2, SIGN_BIT       ; Check the sign of the MSW of the first operand.
        AND  R0, R0, R2
        BRz  OP1_POSITIVE

OP1_NEGATIVE:
        AND  R1, R1, R2         ; Check the sign of the MSW of the second operand.
        BRz  PRINT_RESULT       ; If they aren't the same, then no overflow occurred.

OP2_ALSO_NEGATIVE:              ; Overflow might have occurred.
        AND  R6, R6, R2
        BRnp PRINT_RESULT       ; No overflow.
        BR   OVERFLOWED

OP1_POSITIVE:
        AND  R1, R1, R2         ; Check the sign of the MSW of the second operand.
        BRnp PRINT_RESULT       ; If they're different, then no overflow occurred.

OP2_ALSO_POSITIVE:              ; Overflow may have occurred.
        AND  R6, R6, R2
        BRz  PRINT_RESULT       ; No overflow.

OVERFLOWED:                     ; Overflow occurred, so store that fact.
        AND  R2, R2, #0
        ADD  R2, R2, #1
        ST   R2, OVERFLOW


; Print out the 30-bit result.
PRINT_RESULT:
        LD   R6, MSW_RESULT
        JSR  BEGIN_PRINT_BINARY

        LD   R0, ASCII_SPACE
        OUT

        LD   R6, LSW_RESULT
        JSR  BEGIN_PRINT_BINARY

        LD   R0, CARRY
        BRz  PRINT_OVERFLOW     ; If there wasn't a carry, then just jump to printing overflow.

PRINT_CARRY:
        LEA  R0, CARRY_STR
        PUTS

PRINT_OVERFLOW:
        LD   R0, OVERFLOW
        BRz  FINISH             ; If no overflow occurred, we're done.
        LEA  R0, OVERFLOW_STR
        PUTS

FINISH:
        HALT


; Print the binary representation of the value passed in through R6.
BEGIN_PRINT_BINARY:
        ST   R7, RETURN_ADDRESS ; store return address
        LEA  R3, BITMASKS

PRINT_BINARY:
        LDR  R4, R3, #0         ; Load the current bit mask.
        BRz  END_PRINT_BINARY   ; We've reached the end of the bit masks, sp finish
        AND  R4, R4, R6         ; Check whether the bit is set...
        BRnp PRINT_ONE          ; If yes, print a one, ...

PRINT_ZERO:                     ; Otherwise, print a 0.
        LD   R0, ASCII_ZERO
        OUT

        ADD  R3, R3, #1         ; Increment the bitmask pointer.
        BR   PRINT_BINARY       ; Print the next bit

PRINT_ONE:
        LD   R0, ASCII_ONE
        OUT

        ADD  R3, R3, #1         ; Increment the bitmask pointer.
        BR   PRINT_BINARY       ; Print the next bit

END_PRINT_BINARY:
        LD   R7, RETURN_ADDRESS ; load the correct return address
        RET


; Important bit masks
MASK            .FILL       b0111111111111111
SIGN_BIT        .FILL       b0100000000000000
CARRY_BIT       .FILL       b1000000000000000

; Places to store important information
MSW_RESULT      .FILL       0x0
LSW_RESULT      .FILL       0x0
CARRY           .FILL       0x0
OVERFLOW        .FILL       0x0
RETURN_ADDRESS  .FILL       0x0

; Values to be used to print out.
ASCII_ZERO      .FILL       0x30
ASCII_ONE       .FILL       0x31
ASCII_NEWLINE   .FILL       0xA
ASCII_SPACE     .FILL       0x20
SEPERATOR       .STRINGZ    "=============== ==============="
CARRY_STR       .STRINGZ    " c"
OVERFLOW_STR    .STRINGZ    " v"

; More important bit masks which will be used when printing out the binary representation.
BITMASKS:       .FILL       b100000000000000
                .FILL       b010000000000000
                .FILL       b001000000000000
                .FILL       b000100000000000
                .FILL       b000010000000000
                .FILL       b000001000000000
                .FILL       b000000100000000
                .FILL       b000000010000000
                .FILL       b000000001000000
                .FILL       b000000000100000
                .FILL       b000000000010000
                .FILL       b000000000001000
                .FILL       b000000000000100
                .FILL       b000000000000010
                .FILL       b000000000000001
                .FILL       b000000000000000

.END
