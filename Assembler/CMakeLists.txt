cmake_minimum_required ( VERSION 3.6 )
project ( LC3AS )

set ( CMAKE_CXX_STANDARD 14 )

add_definitions ( "-DPROJECT=\"${PROJECT_NAME}\"" )

set ( CMAKE_CXX_C )

set ( SOURCE_FILES
      main.cpp
      )

set ( ASSEMBLER_SOURCE_FILES
      source/Assembler.cpp
      source/Token_Directive_Orig.cpp
      )

set ( ASSEMBLER_HEADER_FILES
      includes/Assembler.hpp
      includes/Token.hpp
      includes/Tokens/Token_Immediate.hpp
      includes/Tokens/Immediate/Token_Immediate_Decimal.hpp
      includes/Tokens/Immediate/Token_Immediate_Hexadecimal.hpp
      includes/Tokens/Immediate/Token_Immediate_Binary.hpp
      includes/Tokens/Token_Register.hpp
      includes/Tokens/Token_Label.hpp
      includes/Tokens/Directives/Token_Directive.hpp
      includes/Tokens/Directives/Token_Directive_Stringz.hpp
      includes/Tokens/Directives/Token_Directive_Blkw.hpp
      includes/Tokens/Directives/Token_Directive_Fill.hpp
      includes/Tokens/Directives/Token_Directive_Orig.hpp
      includes/Tokens/Directives/Token_Directive_End.hpp
      includes/Tokens/Token_Instruction.hpp
      includes/Tokens/Instructions/Token_Instruction_Br.hpp
      includes/Tokens/Instructions/Token_Instruction_And.hpp
      includes/Tokens/Instructions/Token_Instruction_Add.hpp
      includes/Tokens/Instructions/Token_Instruction_Not.hpp
      includes/Tokens/Instructions/Token_Instruction_Jsr.hpp
      includes/Tokens/Instructions/Token_Instruction_Jsrr.hpp
      includes/Tokens/Instructions/Token_Instruction_Jmp.hpp
      includes/Tokens/Instructions/Token_Instruction_St.hpp
      includes/Tokens/Instructions/Token_Instruction_Str.hpp
      includes/Tokens/Instructions/Token_Instruction_Sti.hpp
      includes/Tokens/Instructions/Token_Instruction_Ld.hpp
      includes/Tokens/Instructions/Token_Instruction_Lea.hpp
      includes/Tokens/Instructions/Token_Instruction_Ldi.hpp
      includes/Tokens/Instructions/Token_Instruction_Ldr.hpp
      includes/Tokens/Instructions/Token_Instruction_Trap.hpp
      includes/Tokens/Instructions/Token_Instruction_Ret.hpp
      includes/Tokens/Traps/Token_Trap_Puts.hpp
      includes/Tokens/Traps/Token_Trap_Halt.hpp
      includes/Tokens/Traps/Token_Trap_Putsp.hpp
      includes/Tokens/Traps/Token_Trap_Getc.hpp
      includes/Tokens/Traps/Token_Trap_Out.hpp
      includes/Tokens/Traps/Token_Trap_In.hpp
      includes/Tokens/Token_String.hpp
      )

add_executable ( LC3AS ${SOURCE_FILES} ${ASSEMBLER_SOURCE_FILES} ${ASSEMBLER_HEADER_FILES} )
