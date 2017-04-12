;
; A program that iteratively calculates the first 3 to 23 fibonacci numbers,
; inclusively, based on the users input. It will keep asking for input until
; an acceptable value is input.
;

; --------------------------------------------------------------
; What each Register is used for in the two parts of the program
;	-- Getting input
;	-- Converting the current fibonacci number to ASCII
;	-- Iteratively finding the Nth Fibonacci Number
; --------------------------------------------------------------

; NOTE: As R1 is used as the input and the counter, we don't want to overwrite at
;	all by anything other than the Iteration part of the program.

;       Getting input			    | Converting to ASCII		    | Iteration
; R0 -- Input and output	        | R0 --	Output			        | R0 -- Unused
; R1 -- The current input	        | R1 --	Unused			        | R1 -- Loop counter
; R2 -- Unused			            | R2 --	The digit position	    | R2 -- Unused
; R3 -- Used to compare values		| R3 --	Number of digits output	| R3 -- The lower fibonacci number
; R4 -- Used to multiply by ten		| R4 --	The current place	    | R4 -- The higher fibonacci number
; R5 -- Used as the character count	| R5 --	The number to convert	| R5 -- Current Fibonacci Number
; R6 -- Used to multiply by ten		| R6 --	The current digit	    | R6 -- Temporary Value
; R7 -- Return address			    | R7 --	Return address		    | R7 -- Return address

.ORIG 0x3000

; Prompt for input, also the entry for the program
OUT_PROMPT:
	LEA R0, PROMPT		// Load the prompt into R0
	PUTS			// Put it to the display

	; Initialise Registers for the beginning of the program
	AND R5, R5, #0		; Reset R5
	ADD R5, R5, #10		; Set R5 to ten, its our character counter
	LD R1, NUMBER		; Load the number into R1
	JSR CLEAR_FLAG		; Clear any flags added


; Repeatedly retrieve a character as input until a newline is pressed, or 10 characters
; have been input
GET_INPUT:
	GETC			; Get a character
	OUT			; Put the character to the output
	ADD R3, R0, #-10	; Check if the input was the newline character
	BRz CHECK_INPUT		; If yes, check what was input
	; Compare input character against 9
	LD R3, ASCII_NINE	; Load the value in ASCII_NINE into R3
	NOT R3, R3		; Start two's compliment
	ADD R3, R3, #1
	ADD R3, R0, R3		; Compare ascii values
	BRp FLAG_THAT		; If the character's ascii value is greater than 9's, flag it
	; Compare input character against 0
	LD R3, ASCII_ZERO	; Load the value of ASCII_ZER0 into R3
	NOT R3, R3		; Start two's compliment
	ADD R3, R3, #1
	ADD R3, R3, R0		; Compare the ascii values
	BRn FLAG_THAT		; If the character's ascii value is less than 0's, flag it

	; So, the character is a digit. Update the number.
	ADD R6, R5, #-10
	BRn CHECK_ZERO
	BRz SET_TO
	BRnzp MULTIPLY_BY_TEN


; Decrement the input counter by one
DECREMENT_INPUT_COUNTER:
	ADD R5, R5, #-1		; Subtract 1 from the character counter stored in R5
	BRz OUT_PROMPT		; We've reached the max character count, so start again.
	BRnzp GET_INPUT


; Set the number to the input
SET_TO:
	ADD R1, R3, #0		; Set the number to the current input
	BRnzp DECREMENT_INPUT_COUNTER


; Multiply the number by 10
MULTIPLY_BY_TEN:
	JSR CHECK_FLAG		; Jump to check if we have flagged something, this will only return here if
				; we have not flagged something.
	ADD R4, R1, R1		; Store  2x R1 in R4
	ADD R6, R4, R4		; Store  4x R1 in R6
	ADD R6, R6, R6		; Store  8x R1 in R6
	ADD R1, R6, R4		; Store 10x R1 in R1
	ADD R1, R1, R3		; Add the current number
	ADD R4, R1, #-16	; We don't want numbers greater than 23.
	ADD R4, R4, #-7		; Because we can't add numbers less than -16, we do this twice
	BRp FLAG_THAT		; If the number is greater than 23, no need to keep going
	BRnzp DECREMENT_INPUT_COUNTER


; Check if we've flagged something, and if so then skip over the input
CHECK_FLAG:
	LD R4, FLAG		; Load the value in FLAG into R4
	ADD R4, R4, #-1		; Add -1 to it
	; If the value in R4 is 0, then the FLAG was set
	BRz DECREMENT_INPUT_COUNTER
	RET			; Return


; Check if the previous number input was a zero, and if so just flag it because its not
; useable.
CHECK_ZERO:
	ADD R6, R1, #-1		; Set R6 to be equal to R1 - 1
	BRn FLAG_THAT		; If R6 is less than 0, then R1 must have been 0, so flag it
	BRnzp MULTIPLY_BY_TEN	; Otherwise, multiply the current number by 10


; Something wrong was entered
FLAG_THAT:
	LEA R4, FLAG		; Load the address of FLAG into R4
	AND R6, R6, #0		; Reset R6
	ADD R6, R6, #1		; Set R6 to be 1
	STR R6, R4, #0		; Store R6's value (1) into the address of R4, with offset 0
	BRnzp DECREMENT_INPUT_COUNTER


; Clear the flag
CLEAR_FLAG:
	LEA R4, FLAG		; Load the address of FLAG into R4
	AND R6, R6, #0		; Reset R6
	STR R6, R4, #0		; Store R6's value (0) into the address of R4, with offset 0
	RET			; Return


; Check the input for anything incorrect
CHECK_INPUT:
	LD R3, FLAG		; Load whats in FLAG to R3
	ADD R3, R3, #-1		; If R3 == 1, then something went wrong
	BRzp OUT_PROMPT		; so start again.
	ADD R4, R1, #-3		; We also don't want numbers less than 3
	BRn OUT_PROMPT		; so start again.

	; Initialise the registers to be used for the loop
	AND R3, R3, #0		; Reset R3 to be used as the lower number
	AND R4, R4, #0		; Reset R4 to be used as the higher number
	ADD R4, R4, #1		; Start at 1
	BRnzp ITERATIVE_FIBONACCI	; Iteratively retrieve each fibonacci number


; Take a number in R5 and convert each digit to ASCII to print to the display
CONVERT_TO_ASCII:
	ST R7, SAVER7		; Store the return address
	ST R4, SAVER4
	ST R3, SAVER3
	LEA R2, NUMBERS		; Load the numbers to use into R2
	AND R6, R6, #0		; The digit in the current place
	AND R3, R3, #0		; The number of digits displayed
; Outer loop to initialise for the inner loop
OUTER_LOOP:
	LDR R4, R2, #0		; Load the current place into R4
	BRz END			; If that number is 0, we're done.
	NOT R4, R4		; Set R4 for two's compliment
	ADD R4, R4, #1
; Inner loop which handles all of the subtracting
INNER_LOOP:
	ADD R5, R5, R4		; Subtract the current number from R5
	BRn CHECK_DIGIT		; and if R5 is now negative, then we've got the digit in R6
	ADD R6, R6, #1		; Otherwise, add 1 the digit in the current place
	BRnzp INNER_LOOP	; And loop again
; We've found what the digit is, so lets check some things
CHECK_DIGIT:
	ADD R6, R6, #0		; If R6 is greater than 0, output it
	BRp OUTPUT
	ADD R3, R3, #0		; Otherwise, if there are no other digits output then skip
	BRz INCREMENT
; Output the digit at the current place
OUTPUT:
	LD R0, ASCII_ZERO	; Load the ascii value for 0 into R0
	ADD R0, R0, R6		; Add the digit to it to get the ascii value for it
	OUT			; Display it
	ADD R3, R3, #1		; We've output a digit, so increment the amount of digits we've output
; The end of a loop, so lets increment a few things
INCREMENT:
	ADD R2, R2, #1		; Increment the pointer to the numbers
	AND R6, R6, #0		; Clear R6 so it can be used again
	Not R4, R4		; Set R4 for two's compliment because we put the number
	ADD R4, R4, #1		; into the negative, so we need to make it positive again
	ADD R5, R5, R4		; Make R5 positive.
	BRnzp OUTER_LOOP	; and loop again.
; We've reached the last digit, so display it and return
END:
	LD R0, ASCII_ZERO	; We want to find the digits ascii value, so add it to
	ADD R0, R0, R5		; the ascii value for 0
	OUT
	LEA R0, SPACE		; Print a space character between each fibonacci number
	PUTS
	LD R3, SAVER3		; Reinitalise the values in R3 and R4
	LD R4, SAVER4		; Because we need them for the next iteration
	LD R7, SAVER7		; Reload the return address
	RET			; and return

; --------------------------------------------
; Iteratively calaculate each fibonacci number
; --------------------------------------------
ITERATIVE_FIBONACCI:
	ADD R5, R4, #0		; Set R5 to equal the higher number
	JSR CONVERT_TO_ASCII	; Convert that fibonacci number to ASCII and print it
	; Put all registers to their correct values
	ADD R6, R4, #0		; Set R6 to equal the value in R4 (higher fibonacci number)
	ADD R4, R3, R6		; Set R4 to equal the lower + higher values
	ADD R3, R6, #0		; Set R3 to equal the previous higher number
	ADD R1, R1, #-1		; Subtract 1 from the loop counter
	BRz FINISH		; We've reached the last number we want, so finish
	BRnzp ITERATIVE_FIBONACCI	; Othwerwise, loop again.


; Finish the program
FINISH:
	HALT


; Strings that will be used throughout the program
PROMPT	.STRINGZ	"\nEnter a number from 3 to 23: "
SPACE	.STRINGZ	" "

NUMBER	.FILL	#0		; The number that we will use as the number of fibonacci numbers we want
FLAG	.FILL	#0		; A way to tell the program we've received input we don't want

; ASCII values that will be used to check input, as well as convert to ASCII
ASCII_ZERO	.FILL	x30
ASCII_THREE	.FILL	x33
ASCII_NINE	.FILL	x39

; Values that we will use to output the current fibonacci number to the screen
NUMBERS	.FILL #10000
	.FILL #1000
	.FILL #100
	.FILL #10
	.FILL #0	; So we can tell when we've reached the last digit

; Save the value of some registers
SAVER7	.FILL	b0
SAVER4	.FILL	#0
SAVER3	.FILL	#0

.END

