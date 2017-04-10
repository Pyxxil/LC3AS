cmake_minimum_required ( VERSION 3.6 )
project ( LC3AS )

set ( CMAKE_CXX_STANDARD 14 )

add_definitions ( "-DPROJECT=\"${PROJECT_NAME}\"" )

set ( SOURCE_FILES
      main.cpp
      )

set ( ASSEMBLER_SOURCE_FILES
      source/Assembler.cpp
      )

set ( ASSEMBLER_HEADER_FILES
      includes/Assembler.hpp
      includes/Token.hpp
      includes/Token_Immediate.hpp
      includes/Token_Immediate_Decimal.hpp
      includes/Token_Immediate_Hexadecimal.hpp
      includes/Token_Immediate_Binary.hpp
      includes/Token_Register.hpp
      includes/Token_Label.hpp
      includes/Token_Directive.hpp
      includes/Token_Directive_Stringz.hpp
      includes/Token_Directive_Blkw.hpp
      includes/Token_Directive_Fill.hpp
      includes/Token_Directive_Orig.hpp
      includes/Token_Directive_End.hpp
      includes/Token_Operator.hpp
      includes/Token_Operator_Br.hpp
      includes/Token_Operator_And.hpp
      includes/Token_Operator_Add.hpp
      includes/Token_Operator_Not.hpp
      includes/Token_Operator_Jsr.hpp
      includes/Token_Operator_Jsrr.hpp
      includes/Token_Operator_Jmp.hpp
      includes/Token_Operator_St.hpp
      includes/Token_Operator_Str.hpp
      includes/Token_Operator_Sti.hpp
      includes/Token_Operator_Ld.hpp
      includes/Token_Operator_Lea.hpp
      includes/Token_Operator_Ldi.hpp
      includes/Token_Operator_Ldr.hpp
      includes/Token_Operator_Trap.hpp
      includes/Token_Trap_Puts.hpp
      includes/Token_Trap_Halt.hpp
      includes/Token_Trap_Putsp.hpp
      includes/Token_Trap_Getc.hpp
      includes/Token_Trap_Out.hpp
      includes/Token_Trap_In.hpp
      includes/Token_String.hpp
      includes/Token_Operator_Ret.hpp
      )

add_executable ( LC3AS ${SOURCE_FILES} ${ASSEMBLER_SOURCE_FILES} ${ASSEMBLER_HEADER_FILES} )
