ADD R1, R2, #0
.SUB R1, R1

INCLUDED_LABEL
        .NEG R1

WARNING_THROWS
        .INCLUDE "include2.h" ; This should complain about recursive includes

