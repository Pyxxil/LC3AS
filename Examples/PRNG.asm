.ORIG 0x3000

    BR main

RSHIFT_SAVER0:
    .FILL #0
RSHIFT_SAVER1:
    .FILL #0

; Right shift the value in R2
RSHIFT:
    ST R0, RSHIFT_SAVER0    ; Save R0 as we'll be using it
    ST R1, RSHIFT_SAVER1    ; Save R1 as we'll be using it
    AND R0, R0, 0
L1:
    ADD R2, R2, -2
    BRz L1_END
    ADD R1, R2, 1
    Brz L1_END
    ADD R0, R0, 1
    BR L1

L1_END:
    ADD R2, R0, 0
    LD R0, RSHIFT_SAVER0
    LD R1, RSHIFT_SAVER1
    RET

XOR_BITMAP:
    .FILL 0b1000000000000000
    .FILL 0b0100000000000000
    .FILL 0b0010000000000000
    .FILL 0b0001000000000000
    .FILL 0b0000100000000000
    .FILL 0b0000010000000000
    .FILL 0b0000001000000000
    .FILL 0b0000000100000000
    .FILL 0b0000000010000000
    .FILL 0b0000000001000000
    .FILL 0b0000000000100000
    .FILL 0b0000000000010000
    .FILL 0b0000000000001000
    .FILL 0b0000000000000100
    .FILL 0b0000000000000010
    .FILL 0b0000000000000001
    .FILL 0

XOR_SAVER0:
    .FILL 0
XOR_SAVER3:
    .FILL 0
XOR_SAVER4:
    .FILL 0
XOR_SAVER5:
    .FILL 0
XOR_SAVER6:
    .FILL 0

; Perform a Bitwise XOR between the values in R1 and R2
; Returns the output in R2
XOR:
    ; Save some registers as we'll be using them
    ST R0, XOR_SAVER0
    ST R3, XOR_SAVER3
    ST R4, XOR_SAVER4
    ST R5, XOR_SAVER5
    ST R6, XOR_SAVER6
    ; As we need to perforom this bitwise, we need a pointer to
    ; some masks.
    LEA R3, XOR_BITMAP
L2: ; Loop through all of the bits
    LDR R4, R3, 0
    BRz L2_END        ; If we loaded a 0, our bitmask is ended.
    AND R5, R4, R2    ; Grab the current bit of R2
    BRz B_NOT_A       ; If it's 0, then check if R1 has a bit there

A_NOT_B:              ; R2 has a bit there, make sure R1 doesn't
    AND R6, R4, R1
    Brnp XOR_NEXT     ; R1 has a bit there, so we XOR gives us 0
    ADD R0, R0, R5    ; Otherwise the bit should be put into the value
    BR XOR_NEXT

B_NOT_A:
    AND R6, R4, R1    ; R2 doesn't have a bit in that position,
    BRz XOR_NEXT      ; so if R1 doesn't, then no bit needs to be
                      ; put in the output
    ADD R0, R0, R6    ; Otherwise, shove a bit there

; Go to the next bit
XOR_NEXT:
    ADD R3, R3, 1
    BR L2

; We're done, so reload the registers we saved, and move the value
; into R2, and return.
L2_END:
    ADD R2, R0, #0
    LD R0, XOR_SAVER0
    LD R3, XOR_SAVER3
    LD R4, XOR_SAVER4
    LD R5, XOR_SAVER5
    LD R6, XOR_SAVER6
    RET

main:
    .SET R1, 0xACE1  // R1 is start_state
    ADD R2, R1, 0    // R2 is lfsr
    AND R5, R5, 0    // R3 is period
    .SET R3, 0xB400

L3:
    AND R4, R2, 1    // LSB
    JSR RSHIFT
    ADD R4, R4, 0
    BRz IF_END
    JSR XOR

IF_END:
    ADD R5, R5, 1
    ADD R6, R2, 0
    .NEG R6
    ADD R6, R6, R1
    BRnp L2
DONE:
    HALT

.END
