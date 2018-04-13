;
; This program will repeatedly ask the user to input a 2 digit number
; between 00 and 99 (to input a number between 0 and 9, they need to
; prefix it with a 0, e.g. 09)
;
; One assumption is that the user will only enter 2 digit's (as in
; they won't enter 'a')
;

;
; Register Usage:
; R0 | Input/Output
; R1 | The number the user is trying to guess
; R2 | The number in the 10's column
; R3 | The number in the 1's column
; R4 | The inverse of 0x30
; R5 | Temporary values
; R6 & R7 | Not used

.ORIG x3000

	LD R1, NUMBER		; The number to guess
	LD R4, ASCII_OFFSET

LOOP
	LEA R0, PROMPT		; Print the prompt to the terminal
	PUTS
; Grab first character
	GETC
	OUT			; We need to output it, as GETC doesn't
	ADD R2, R0, R4		; This gives R2 the decimal value of the
				; input character (0 if '0' is input)
; Grab second character
	GETC
	OUT
	ADD R3, R0, R4
; Print a new line, so it looks nicer
	AND R0, R0, 0
	ADD R0, R0, 10
	OUT
; Multiply by 10
	ADD R2, R2, R2		; R2 = 2R2
	ADD R5, R2, R2		; R5 = 4R2 (R2 being the original value)
	ADD R5, R5, R5		; R5 = 8R2 (R2, again, the original value)
	ADD R2, R5, R2		; R2 = 2R2 + 8R2 = 10R2
; Make actual value
	ADD R5, R2, R3		; This sets R5 to be, e.g. 12 if the user
				; enter's '1' and '2'
; Negate the entered value (using 2's compliment)
	NOT R5, R5		; Invert all bits
	ADD R5, R5, 1		; Add 1 to it
	
	ADD R5, R5, R1		; Perform R1 - R5 to check if they are equal,
				; greater or less.
	BRz FINISH		; If the CC is 'Z', then R1 - R5 = 0, and are
				; therefore the same value
	BRn GREATER_THAN		; If the CC is 'N', then R1 - R5 > 0, and
				; therefore the number entered was greater

; The value entered was less than the number
LESS_THAN
	LEA R0, LESS
	PUTS
	BR LOOP

; The value entered was greater than the number
GREATER_THAN
	LEA R0, GREATER
	PUTS
	BR LOOP
	
FINISH
	LEA R0, CORRECT
	PUTS
	HALT
	
NUMBER				; The number the user needs to guess
	.FILL 6
	
PROMPT
	.STRINGZ "Guess a number (between 00 and 99): "

CORRECT
	.STRINGZ "Correct!\n"

LESS
	.STRINGZ "Less\n"

GREATER
	.STRINGZ "Greater\n"

ASCII_OFFSET
	.FILL 0xFFD0		; This is -0x30
.END











