
set(LEXER_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/String_Matcher.cpp
    )

set(LEXER_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Lexer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Line.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/String_Matcher.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/LexHelper.hpp
    )

set(TOKENS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Token_Directive.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Directives/Token_Directive_Blkw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Directives/Token_Directive_End.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Directives/Token_Directive_Fill.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Directives/Token_Directive_Orig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Directives/Token_Directive_Stringz.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Token_Immediate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Immediates/Token_Immediate_Binary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Immediates/Token_Immediate_Decimal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Immediates/Token_Immediate_Hexadecimal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Immediates/Token_Immediate_Octal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Token_Instruction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Add.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_And.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Br.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Jmp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Jsr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Jsrr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Ld.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Ldi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Ldr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Lea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Not.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Ret.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_St.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Sti.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Str.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Instructions/Token_Instruction_Trap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Token_Label.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Token_Register.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Token_String.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Traps/Token_Trap_Halt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Traps/Token_Trap_Getc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Traps/Token_Trap_In.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Traps/Token_Trap_Out.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Traps/Token_Trap_Puts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Traps/Token_Trap_Putsp.cpp
    )

set(TOKENS_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Symbol.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Token.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Token_Directive.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Directives/Token_Directive_Blkw.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Directives/Token_Directive_End.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Directives/Token_Directive_Fill.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Directives/Token_Directive_Orig.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Directives/Token_Directive_Stringz.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Token_Immediate.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Immediates/Token_Immediate_Binary.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Immediates/Token_Immediate_Decimal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Immediates/Token_Immediate_Hexadecimal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Immediates/Token_Immediate_Octal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Token_Instruction.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Add.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_And.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Br.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Jmp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Jsr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Jsrr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Ld.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Ldi.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Ldr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Lea.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Not.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Ret.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_St.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Sti.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Str.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Instructions/Token_Instruction_Trap.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Token_Label.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Token_Register.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Token_String.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Traps/Token_Trap_Getc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Traps/Token_Trap_Halt.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Traps/Token_Trap_In.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Traps/Token_Trap_Out.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Traps/Token_Trap_Puts.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Traps/Token_Trap_Putsp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Tokens.hpp
    )

if (ALLOW_EXTRAS)
    set (TOKENS_SOURCE_FILES
        "${TOKENS_SOURCE_FILES}"
        ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Addons/Token_Addon_Char.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Addons/Token_Addon_Include.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Addons/Token_Addon_Lshift.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Addons/Token_Addon_Neg.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Addons/Token_Addon_Set.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Tokens/Addons/Token_Addon_Sub.cpp
        )

    set (TOKENS_HEADER_FILES
        "${TOKENS_HEADER_FILES}"
        ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Addons/Token_Addon_Char.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Addons/Token_Addon_Include.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Addons/Token_Addon_Lshift.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Addons/Token_Addon_Neg.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Addons/Token_Addon_Set.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Tokens/Addons/Token_Addon_Sub.hpp
        )
endif ()

set(PARSER_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Parser/Parser.cpp
    )

set(PARSER_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Parser.hpp
    )

set(DIAGNOSTICS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Diagnostics/Diagnostics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Diagnostics/Context.cpp
    )

set(DIAGNOSTICS_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Diagnostics.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Context.hpp
    )

set(CONFIGURATION_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Config/Configuration.cpp
    )

set(CONFIGURATION_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Configuration.hpp
    )

set(LIBRARY_SOURCE_FILES
    "${LIBRARY_SOURCE_FILES}"
    "${PARSER_SOURCE_FILES}"
    "${LEXER_SOURCE_FILES}"
    "${DIAGNOSTICS_SOURCE_FILES}"
    "${CONFIGURATION_SOURCE_FILES}"
    "${TOKENS_SOURCE_FILES}"
    PARENT_SCOPE
    )

set(LIBRARY_HEADER_FILES
    "${LIBRARY_HEADER_FILES}"
    "${PARSER_HEADER_FILES}"
    "${LEXER_HEADER_FILES}"
    "${DIAGNOSTICS_HEADER_FILES}"
    "${CONFIGURATION_HEADER_FILES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Console.hpp"
    "${TOKENS_HEADER_FILES}"
    PARENT_SCOPE
    )

set(LIBRARY_INCLUDE_DIRECTORIES
    "${LIBRARY_INCLUDE_DIRECTORIES}"
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes/
    ${CMAKE_CURRENT_SOURCE_DIR}/Tokens/
    PARENT_SCOPE
    )
