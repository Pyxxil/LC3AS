cmake_minimum_required ( VERSION 3.6 )
project ( LC3Assembler )

include_directories ( includes )

set ( ASSEMBLER_SOURCE_FILES
      source/Assembler.cpp
      source/Tokens/Token_Label.cpp
      source/Tokens/Token_String.cpp
      source/Tokens/Token_Register.cpp
      source/Tokens/Token_Instruction.cpp
      source/Tokens/Token_Immediate.cpp
      source/Tokens/Immediate/Token_Immediate_Hexadecimal.cpp
      source/Tokens/Immediate/Token_Immediate_Decimal.cpp
      source/Tokens/Immediate/Token_Immediate_Binary.cpp
      source/Tokens/Immediate/Token_Immediate_Octal.cpp
      source/Tokens/Directives/Token_Directive_Stringz.cpp
      source/Tokens/Directives/Token_Directive_Orig.cpp
      source/Tokens/Directives/Token_Directive_Fill.cpp
      source/Tokens/Directives/Token_Directive_Blkw.cpp
      source/Tokens/Directives/Token_Directive_End.cpp
      source/Tokens/Traps/Token_Trap_Putsp.cpp
      source/Tokens/Traps/Token_Trap_Getc.cpp
      source/Tokens/Traps/Token_Trap_Halt.cpp
      source/Tokens/Traps/Token_Trap_Puts.cpp
      source/Tokens/Traps/Token_Trap_Out.cpp
      source/Tokens/Traps/Token_Trap_In.cpp
      source/Tokens/Instructions/Token_Instruction_Jsrr.cpp
      source/Tokens/Instructions/Token_Instruction_Trap.cpp
      source/Tokens/Instructions/Token_Instruction_Add.cpp
      source/Tokens/Instructions/Token_Instruction_And.cpp
      source/Tokens/Instructions/Token_Instruction_Sti.cpp
      source/Tokens/Instructions/Token_Instruction_Jmp.cpp
      source/Tokens/Instructions/Token_Instruction_Jsr.cpp
      source/Tokens/Instructions/Token_Instruction_Ldi.cpp
      source/Tokens/Instructions/Token_Instruction_Ldr.cpp
      source/Tokens/Instructions/Token_Instruction_Lea.cpp
      source/Tokens/Instructions/Token_Instruction_Not.cpp
      source/Tokens/Instructions/Token_Instruction_Ret.cpp
      source/Tokens/Instructions/Token_Instruction_Str.cpp
      source/Tokens/Instructions/Token_Instruction_Br.cpp
      source/Tokens/Instructions/Token_Instruction_Ld.cpp
      source/Tokens/Instructions/Token_Instruction_St.cpp
      )

set ( ASSEMBLER_HEADER_FILES
      includes/Assembler.hpp
      includes/Token.hpp
      includes/Tokens/Token_Immediate.hpp
      includes/Tokens/Token_Register.hpp
      includes/Tokens/Token_String.hpp
      includes/Tokens/Token_Label.hpp
      includes/Tokens/All_Tokens.hpp
      includes/Tokens/Immediate/Token_Immediate_Hexadecimal.hpp
      includes/Tokens/Immediate/Token_Immediate_Decimal.hpp
      includes/Tokens/Immediate/Token_Immediate_Binary.hpp
      includes/Tokens/Immediate/Token_Immediate_Octal.hpp
      includes/Tokens/Token_Directive.hpp
      includes/Tokens/Directives/Token_Directive_Stringz.hpp
      includes/Tokens/Directives/Token_Directive_Blkw.hpp
      includes/Tokens/Directives/Token_Directive_Fill.hpp
      includes/Tokens/Directives/Token_Directive_Orig.hpp
      includes/Tokens/Directives/Token_Directive_End.hpp
      includes/Tokens/Traps/Token_Trap_Putsp.hpp
      includes/Tokens/Traps/Token_Trap_Puts.hpp
      includes/Tokens/Traps/Token_Trap_Halt.hpp
      includes/Tokens/Traps/Token_Trap_Getc.hpp
      includes/Tokens/Traps/Token_Trap_Out.hpp
      includes/Tokens/Traps/Token_Trap_In.hpp
      includes/Tokens/Token_Instruction.hpp
      includes/Tokens/Instructions/Token_Instruction_Jsrr.hpp
      includes/Tokens/Instructions/Token_Instruction_Trap.hpp
      includes/Tokens/Instructions/Token_Instruction_And.hpp
      includes/Tokens/Instructions/Token_Instruction_Add.hpp
      includes/Tokens/Instructions/Token_Instruction_Not.hpp
      includes/Tokens/Instructions/Token_Instruction_Jsr.hpp
      includes/Tokens/Instructions/Token_Instruction_Jmp.hpp
      includes/Tokens/Instructions/Token_Instruction_Str.hpp
      includes/Tokens/Instructions/Token_Instruction_Sti.hpp
      includes/Tokens/Instructions/Token_Instruction_Lea.hpp
      includes/Tokens/Instructions/Token_Instruction_Ldi.hpp
      includes/Tokens/Instructions/Token_Instruction_Ldr.hpp
      includes/Tokens/Instructions/Token_Instruction_Ret.hpp
      includes/Tokens/Instructions/Token_Instruction_Br.hpp
      includes/Tokens/Instructions/Token_Instruction_Ld.hpp
      includes/Tokens/Instructions/Token_Instruction_St.hpp
      )

set ( DEBUG_FLAGS ${COMPILE_FLAGS} -O0 -g -Werror -pedantic -Wshadow -Wnon-virtual-dtor -Wconversion )
set ( DEBUG_FLAGS ${DEBUG_FLAGS} -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Weffc++ )
set ( DEBUG_FLAGS ${DEBUG_FLAGS} -Wsign-conversion -Wno-error=effc++ -Wno-error=sign-conversion -Wno-error=conversion )

add_library ( ${PROJECT_NAME} STATIC ${ASSEMBLER_SOURCE_FILES} ${ASSEMBLER_HEADER_FILES} )

target_compile_options ( ${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>" )
target_compile_options ( ${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>" )
