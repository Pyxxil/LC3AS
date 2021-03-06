# What is it?

This is, currently, an LC3-Assembler. It is not currently completely compatible with the original Assembler, however I plan for it to be at some point (see below for known incompatibilites).

At some later point I will hopefully also be adding a Simulator and an Editor (this is the current W.I.P).

## Assembler

To build the assembler, both the Assembler and it's libraries need to be compiled. This should be the default configuration when building with cmake:

```bash
cd /path/to/download
cmake # -DUSE_ADDONS=ON for addons (described below)
make -j $NUMBER_OF_JOBS  # Where you replace $NUMBER_OF_JOBS with
                         # whatever value you'd like (though, the number
                         # of cores you have is a good choice)
./LC3AS [options] <input files>
```

### Addons

I've added some extra things that the Assembler can use, however they are compile time constants, so if you change your mind you need to recompile with the new options.

These are:
 - Character literals : A character surrounded by single quotes
 - .LSHIFT Rs, imm : Shift the register supplied as Rs by the provided immediate value (between 0 and 15, inclusive)
    + At some point I may allow a shift greater than 15, which will just be the same as clearing the register.
 - .INCLUDE "String" : Include the file in the string
 - .SET Rs, imm : Set the value in Rs to the value supplied by imm
 - .NEG Rs : Perform 2's compliment on the supplied register
 - .SUB Rd, Rs[, Rt] : Subtract Rt from Rs if supplied, otherwise subtract Rs from Rd. Put the result into Rd
 - ADD and AND modes for two operands, e.g. ADD R0, R1 or AND R0, #0, which will
 use the dest register as one of the source registers

### Known Incompatibilities

 - The original Assembler allows the entire program be on one line, but this currently only allows a single instruction per line (with an optional starting Label).
