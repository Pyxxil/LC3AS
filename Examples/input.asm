.ORIG x3000

;
; Performs a function similar to the 'input' function in Python.
; It only allows a maximum of 20 characters to be input.
;

; Register Usage
; R0 | Input/Output
; R1 | String pointer
; R2 | Counter
; R3 & R4 & R5 & R6 & R7 | Unused
;

LEA R1, STRING          ; We need a pointer to the string
LD R2, COUNT            ; Our counter

LEA R0, BEGIN_PROMPT
PUTS

LOOP
    GETC                ; Grab the next character
    OUT                 ; Display the last input character
    ADD R3, R0, -10     ; Do R0 - 10 (10 being the new line character)
    BRz FINISH          ; If the result is 0, then the user pressed
                        ; enter, so we want to finish
    STR R0, R1, 0       ; Store the inputted character
    ADD R1, R1, 1       ; Go to the next position in our string
    ADD R2, R2, -1      ; Decrement the counter
    BRz EARLY_FINISH    ; If the counter is 0, then we're done
    BR LOOP             ; Otherwise, grab another character

; We finished before the user pressed enter, so lets print a new line now
EARLY_FINISH
    AND R0, R0, 0
    ADD R0, R0, 10
    OUT
; Print roughly the following string "You entered the following string: '......'"
FINISH
    LEA R0, PROMPT      ; Print a prompt
    PUTS
    LEA R0, STRING      ; Print the inputted string
    PUTS
    LEA R0, END_PROMPT  ; Print the end of the prompt
    PUTS
    HALT

BEGIN_PROMPT
    .STRINGZ "Enter a string: "

PROMPT
    .STRINGZ "You entered the following string: '"
END_PROMPT
    .STRINGZ "'"

STRING
    .BLKW 21
COUNT
    .FILL 20

.END