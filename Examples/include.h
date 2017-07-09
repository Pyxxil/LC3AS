ADD R1, R2, #0
.SUB R1, R1

INCLUDED_LABEL
        .NEG R1

;        .INCLUDE "Features.asm" ; This should complain about recursive includes
