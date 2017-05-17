;
; A program that recursively calaculates and outputs a given amount of fibonacci numbers
; (3 to 23 inclusively).
;
; --------------------------------------------------------------
; What each Register is used for in the three parts of the program
;        -- Getting input
;        -- Converting the current fibonacci number to ASCII
;        -- Recursively finding the Nth Fibonacci Number
; --------------------------------------------------------------
;
;       Getting input               | Converting to ASCII           | Recursion
; R0 -- Input and output            | R0 -- Output                  | R0 -- Result stack pointer
; R1 -- The current input           | R1 -- Unused                  | R1 -- Unused
; R2 -- Unused                      | R2 -- The digit position      | R2 -- Stack pointer
; R3 -- Used to compare values      | R3 -- Number of digits output | R3 -- The current N
; R4 -- Used to multiply by ten     | R4 -- The current place       | R4 -- Temporary Value stack pointer
; R5 -- Used as the character count | R5 -- The number to convert   | R5 -- The fibonacci number
; R6 -- Used to multiply by ten     | R6 -- The current digit       | R6 -- Temporary Values
; R7 -- Return address              | R7 -- Return address          | R7 -- Return address
;


.ORIG 0x3000


; Start by putting the prompt to the screen.
OUT_PROMPT:
        LEA R0, PROMPT                  ; Load the prompt into R0.
        PUTS                            ; Put it to the display.

        ; Initialise Registers for the first part of the program.
        AND R5, R5, #0                  ; Reset R5.
        ADD R5, R5, #10                 ; Set R5 to ten, its our character counter.
        LD R1, NUMBER                   ; Load the number into R1.
        JSR CLEAR_FLAG                  ; Clear any flags added.


; Repeatedly retrieve a character as input until a newline is pressed, or 10
; characters have been input.
GET_INPUT:
        GETC                            ; Get a character.
        OUT                             ; Put the character to the output.
        ADD R3, R0, #-10                ; Check if the input was the newline character.
        BRz CHECK_INPUT                 ; If yes, check what the input was.

        ; Compare input character against 9.
        LD R3, NINE                     ; Load the value in NINE into R3.
        NOT R3, R3                      ; Start two's compliment.
        ADD R3, R3, #1
        ADD R3, R0, R3                  ; Compare ascii values.
        BRp FLAG_THAT                   ; If the character's ascii value is greater
                                        ; than 9's, flag it.
        ; Compare input character against 0.
        LD R3, ZERO                     ; Load the value of ZER0 into R3.
        NOT R3, R3                      ; Negate R3.
        ADD R3, R3, #1
        ADD R3, R3, R0                  ; Compare the ascii values.
        BRn FLAG_THAT                   ; If the character's ascii value is less than 0's,
                                        ; flag it.
        ; So, the character is a digit. Update the number.
        ADD R6, R5, #-10                ; Check the amount of characters input.
        BRn CHECK_ZERO                  ; Make sure that if the first character was a 0,
                                        ; pass it.
        BRz SET_TO                      ; If this is the first character, then set the
                                        ; count to this number.
        BRnzp MULTIPLY_BY_TEN           ; Always multiply the current number by ten if
                                        ; it doesn't get updated by the above.

; The user entered too many characters, so print a prompt telling them so, and go back
; to the beginning of the program.
TOO_MANY_CHARS:
        LEA R0, TOO_MANY
        PUTS
        BR OUT_PROMPT

TOO_MANY    .STRINGZ "\nEntered too many characters (Max 10).\n"

; Decrement the input counter by one.
DECREMENT_INPUT_COUNTER:
        ADD R5, R5, #-1                 ; Subtract 1 from the loop counter stored in R5.
        BRz TOO_MANY_CHARS              ; We've reached the max character count, so start
                                        ; again.
        BR GET_INPUT


; Set the number to the input.
SET_TO:
        ADD R1, R3, #0                  ; Set the number to the current input.
        BRnzp DECREMENT_INPUT_COUNTER


; Multiply the number by 10
MULTIPLY_BY_TEN:
        JSR CHECK_FLAG                  ; Jump to check if we have flagged something,
                                        ; this will only return here if we have not flagged something.
        ADD R4, R1, R1                  ; Store  2x R1 in R4.
        ADD R6, R4, R4                  ; Store  4x R1 in R1.
        ADD R6, R6, R6                  ; Store  8x R1 in R1.
        ADD R1, R6, R4                  ; Store 10x R1 in R1.
        ADD R1, R1, R3                  ; Add the current number.
        ADD R4, R1, #-16                ; We don't want numbers greater than 23.
        ADD R4, R4, #-7                 ; Because we can't add numbers less than -16,
                                        ; we do this twice.
        BRp FLAG_THAT                   ; If the number is greater than 23, no need to
                                        ; keep going.
        BR DECREMENT_INPUT_COUNTER


; Check if we've flagged something, and if so then skip over the input.
CHECK_FLAG:
        LD R4, FLAG                     ; Load the value in FLAG into R4.
        ADD R4, R4, #-1                 ; Add -1 to it.
        ; If the value in R4 is 0, then the FLAG was set.
        BRz DECREMENT_INPUT_COUNTER
        RET


; Check if the previous number input was a zero, and if so just flag it because
; its not useable.
CHECK_ZERO:
        ADD R6, R1, #-1                 ; Set R6 to be equal to R1 - 1.
        BRn FLAG_THAT                   ; If R6 is less than 0, then R1 must have been 0,
                                        ; so flag it.
        BRnzp MULTIPLY_BY_TEN           ; Otherwise, multiply the current number by 10.


; Something incorrect was entered.
FLAG_THAT:
        LEA R4, FLAG                    ; Load the address of FLAG into R4.
        AND R6, R6, #0                  ; Reset R6.
        ADD R6, R6, #1
        STR R6, R4, #0                  ; Set the flag.
        BR DECREMENT_INPUT_COUNTER


; Clear the flag
CLEAR_FLAG:
        LEA R4, FLAG                    ; Load the address of FLAG into R4.
        AND R6, R6, #0                  ; Reset R6.
        STR R6, R4, #0                  ; Store R6's value (0) into the address of R4,
                                        ; with offset 0.
        RET


; Check the input for anything incorrect
CHECK_INPUT:
        LD R3, FLAG                     ; Load whats in FLAG to R3.
        ADD R3, R3, #-1                 ; If R3 == 1, then something went wrong
        BRzp OUT_PROMPT                 ; so start again.
        ADD R4, R1, #-3                 ; We also don't want numbers less than 3
        BRn OUT_PROMPT                  ; so start again.
        ; Note, we don't have to check if the number is greater than 23 because
        ; we already did that in MULTIPLY_BY_TEN.
        ST R1, NUMBER                   ; Store the value of R1 into the address.
        BRnzp INIT_LOOP                 ; Get ready to find the Nth Fibonacci number.


; Take a number in R5 and convert each digit to ASCII to print to the display.
CONVERT_TO_ASCII:
        ST R7,  SAVER7                  ; Store the return address.
        LEA R2, NUMBERS                 ; Load the numbers to use into R2.
        AND R6, R6, #0                  ; The digit in the current place.
        AND R3, R3, #0                  ; The number of digits displayed.

; Outer loop to initialise for the inner loop
OUTER_LOOP:
        LDR R4, R2, #0                  ; Load the current place into R4.
        BRz END                         ; If that number is 0, we're done.
        NOT R4, R4                      ; Set R4 for two's compliment.
        ADD R4, R4, #1

; Inner loop which handles all of the subtracting
INNER_LOOP:
        ADD R5, R5, R4                  ; Subtract the current number from R5.
        BRn CHECK_DIGIT                 ; and if R5 is now negative, then we've got the
                                        ; digit in R6.
        ADD R6, R6, #1                  ; Otherwise, add 1 the digit in the current place.
        BRnzp INNER_LOOP                ; And loop again.

; We've found what the digit is, so lets check some things.
CHECK_DIGIT:
        ADD R6, R6, #0                  ; If R6 is greater than 0, output it.
        BRp OUTPUT
        ADD R3, R3, #0                  ; Otherwise, if there are no other digits output
        BRz INCREMENT                   ; then skip.

; Output the digit at the current place.
OUTPUT:
        LD R0,  ZERO                    ; Load the ascii value for 0 into R0.
        ADD R0, R0, R6                  ; Add the digit to get the ascii value for it.
        OUT                             ; Display it.
        ADD R3, R3, #1                  ; We've output a digit, so increment the amount
                                        ; of digits we've output.

; The end of a loop, so lets increment a few things.
INCREMENT:
        ADD R2, R2, #1                  ; Increment the pointer to the numbers.
        AND R6, R6, #0                  ; Clear R6 so it can be used again.
        NOT R4, R4                      ; Set R4 for two's compliment because we put the
        ADD R4, R4, #1                  ; number into the negative, so we need to make it
                                        ; positive again.
        ADD R5, R5, R4                  ; Make R5 positive.
        BRnzp OUTER_LOOP

; We've reached the last digit, so display it and return.
END:
        LD R0, ZERO                     ; We want to find the digits ascii value, so add
        ADD R0, R0, R5                  ; it to the ascii value for 0.
        OUT
        LD R0, SPACE                    ; Print a space character between each fibonacci
        OUT                             ; number.
        LD R7, SAVER7                   ; Reload the return address.
        RET


; These have to go here, otherwise their offset will be too great for the 9 bit
; offset that can be provided to most of the memory access/writer instructions.
SAVER7      .FILL 0

; Strings that will be used throughout the program
PROMPT      .STRINGZ "\nEnter a number from 3 to 23: "

SPACE       .FILL 0x20                  ; A space character has the ascii value of 0x20, too large
                                        ; for an immediate offset to ADD.

NUMBER      .FILL 0                     ; The number that we will use as the number of fibonacci
                                        ; numbers we want.
FLAG        .FILL 0                     ; A way to tell the program we've received input we don't want.

; ASCII values that will be used to check input, as well as convert to ASCII.
ZERO        .FILL #48
THREE       .FILL #51
NINE        .FILL #57

; Values that we will use to output the current fibonacci number to the screen.
NUMBERS     .FILL #10000
            .FILL #1000
            .FILL #100
            .FILL #10
            .FILL #0                    ; So we can tell when we've reached the last digit.


;       Getting input               | Converting to ASCII           | Recursion
; R0 -- Input and output            | R0 -- Output                  | R0 -- Result stack pointer
; R1 -- The current input           | R1 -- Unused                  | R1 -- Unused
; R2 -- Unused                      | R2 -- The digit position      | R2 -- Stack pointer
; R3 -- Used to compare values      | R3 -- Number of digits output | R3 -- The current N
; R4 -- Used to multiply by ten     | R4 -- The current place       | R4 -- Temporary Value stack pointer
; R5 -- Used as the character count | R5 -- The number to convert   | R5 -- The fibonacci number
; R6 -- Used to multiply by ten     | R6 -- The current digit       | R6 -- Temporary Values
; R7 -- Return address              | R7 -- Return address          | R7 -- Return address
; ---------------------------------------------------;
; Recursive function to find the Nth fibonacci number;
;----------------------------------------------------;
RECURSIVE_FIBONACCI:
        STR R7, R2, #0                  ; Store return address.
        ADD R2, R2, #1                  ; Increment stack pointer.
        ADD R6, R3, #-3                 ; If the current number is 0,
        BRn BASE_CASE                   ; then we've reached one of the base cases.

        ADD R3, R3, #-1                 ; N - 1.
        STR R3, R4, #0                  ; Save R3 to temp.
        ADD R4, R4, #1                  ; Increment the Temporary stack pointer.
        JSR RECURSIVE_FIBONACCI         ; Recurse with N - 1.

        ADD R4, R4, #-1                 ; Decrement the Temporary stack pointer.
        LDR R3, R4, #0                  ; Retrieve N from the stack.
        ADD R3, R3, #-1                 ; N = N - 2.
        JSR RECURSIVE_FIBONACCI         ; Recurse with N - 2.

        ADD R0, R0, #-1                 ; Decrement stack pointer.
        LDR R3, R0, #0                  ; Load result of Fib(N - 1).
        ADD R0, R0, #-1                 ; Decrement stack pointer.
        LDR R5, R0, #0                  ; Load result of Fib(N - 2).
        ADD R5, R5, R3                  ; Add the recursive results together
        BR RETURN                       ; and return.

BASE_CASE:
        AND R5, R5, #0                  ; We hit a base case,
        ADD R5, R5, #1                  ; so we want to return 1.

; We've found this fibonacci number, so return to the last return address stored
; in the stack.
RETURN:
        ADD R2, R2, #-1                 ; Decrement the stack pointer.
        LDR R7, R2, #0                  ; Load return address from stack.
        STR R5, R0, #0                  ; Save the result to the Result stack.
        ADD R0, R0, #1                  ; Increment result stack.
        RET                             ; Result is in Result stack, so return.


; Initialise some registers.
INIT_LOOP:
        LD R3, COUNT                    ; The fibonacci sequence number we want to compute.
; Start looping upwards towards the number input by the user.
LOOP:
        LEA R2, STACK                   ; R2 is now the stack pointer.
        LEA R0, RESULT_STACK            ; R0 is now the pointer to the result stack.
        LEA R4, TEMP_VAL_STACK          ; R4 is now the pointer to the temporary value stack.
        JSR RECURSIVE_FIBONACCI         ; Find the fibonacci number.

        ADD R0, R0,#-1                  ; Decrement the stack pointer, as the result is at
                                        ; the top.
        LDR R5, R0,#0                   ; Load R5 from stack.
        JSR CONVERT_TO_ASCII            ; Display the fibonacci number.
        LD R3, COUNT                    ; Load the current count into R3.
        LD R6, NUMBER                   ; Load the original input into R6.
        NOT R6, R6                      ; Set R6 for twos compliment.
        ADD R6, R6, #1
        ADD R6, R6, R3                  ; Check if R3 is now equal to the input.
        BRz FINISH                      ; If so, then finish the program.
        ADD R3, R3, #1                  ; Otherwise, add 1 to the count
        ST R3, COUNT                    ; store the new count into NUMBER
        BRnzp LOOP                      ; and loop again.

; We've finished finding the fibonacci numbers, so end the program.
FINISH: HALT

; The loop count (will also be used to find the fibonacci number we want).
COUNT       .FILL 1
SAVER       .FILL SAVER7
LOOPER      .FILL INIT_LOOP

; Our stack pointers.
STACK               .BLKW #100          ; Return address and frame stack pointer.
RESULT_STACK        .BLKW #100          ; Result stack pointer, holds all results during
                                        ; the recursion.
TEMP_VAL_STACK      .BLKW #100          ; To store all temporary values we use.

.END

