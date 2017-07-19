#ifndef TOKENS_HPP
#define TOKENS_HPP

#include "Tokens/Token_Label.hpp"
#include "Tokens/Token_Register.hpp"
#include "Tokens/Token_String.hpp"

#include "Tokens/Token_Directive_Blkw.hpp"
#include "Tokens/Token_Directive_End.hpp"
#include "Tokens/Token_Directive_Fill.hpp"
#include "Tokens/Token_Directive_Orig.hpp"
#include "Tokens/Token_Directive_Stringz.hpp"

#include "Tokens/Token_Immediate_Binary.hpp"
#include "Tokens/Token_Immediate_Decimal.hpp"
#include "Tokens/Token_Immediate_Hexadecimal.hpp"
#include "Tokens/Token_Immediate_Octal.hpp"

#include "Tokens/Token_Trap_Getc.hpp"
#include "Tokens/Token_Trap_Halt.hpp"
#include "Tokens/Token_Trap_In.hpp"
#include "Tokens/Token_Trap_Out.hpp"
#include "Tokens/Token_Trap_Puts.hpp"
#include "Tokens/Token_Trap_Putsp.hpp"

#include "Tokens/Token_Instruction_Add.hpp"
#include "Tokens/Token_Instruction_And.hpp"
#include "Tokens/Token_Instruction_Br.hpp"
#include "Tokens/Token_Instruction_Jmp.hpp"
#include "Tokens/Token_Instruction_Jsr.hpp"
#include "Tokens/Token_Instruction_Jsrr.hpp"
#include "Tokens/Token_Instruction_Ld.hpp"
#include "Tokens/Token_Instruction_Ldi.hpp"
#include "Tokens/Token_Instruction_Ldr.hpp"
#include "Tokens/Token_Instruction_Lea.hpp"
#include "Tokens/Token_Instruction_Not.hpp"
#include "Tokens/Token_Instruction_Ret.hpp"
#include "Tokens/Token_Instruction_St.hpp"
#include "Tokens/Token_Instruction_Sti.hpp"
#include "Tokens/Token_Instruction_Str.hpp"
#include "Tokens/Token_Instruction_Trap.hpp"

#ifdef INCLUDE_ADDONS
#include "Tokens/Token_Addon_Char.hpp"
#include "Tokens/Token_Addon_Include.hpp"
#include "Tokens/Token_Addon_Lshift.hpp"
#include "Tokens/Token_Addon_Neg.hpp"
#include "Tokens/Token_Addon_Set.hpp"
#include "Tokens/Token_Addon_Sub.hpp"
#endif

#endif // TOKENS_HPP
