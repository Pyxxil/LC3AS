#include "Tokens/Token.hpp"

#include "LexHelper.hpp"

#include "Diagnostics.hpp"

/*! Used in reporting errors (to give type information).
 *
 * @return The type string
 */
std::string Token::deduce_type() const {
  switch (type()) {
  case OP_BR:
  case OP_ST:
  case OP_LD:
  case OP_LDR:
  case OP_LEA:
  case OP_LDI:
  case OP_NOT:
  case OP_RTI:
  case OP_STI:
  case OP_STR:
  case OP_ADD:
  case OP_AND:
  case OP_JSR:
  case OP_RET:
  case OP_JMP:
  case OP_JSRR:
  case OP_TRAP:
#ifdef INCLUDE_ADDONS
  case ADDON_JMPT:
#endif
    return std::string{"Instruction"};
  case DIR_END:
  case DIR_FILL:
  case DIR_BLKW:
  case DIR_ORIG:
  case DIR_STRINGZ:
#ifdef INCLUDE_ADDONS
  case ADDON_NEG:
  case ADDON_SUB:
  case ADDON_SET:
  case ADDON_LSHIFT:
#endif
    return std::string{"Directive"};
  case TRAP_IN:
  case TRAP_OUT:
  case TRAP_PUTS:
  case TRAP_GETC:
  case TRAP_HALT:
  case TRAP_PUTSP:
    return std::string{"Trap Routine"};
  case REGISTER:
    return std::string{"Register"};
  case LABEL:
    return std::string{"Label"};
  case IMMEDIATE:
    return std::string{"Immediate Value"};
  case _STRING:
    return std::string{"String Literal"};
  default:
    return std::string{"None Type"};
  }
}

void Token::expected(Expected expect) const {
  Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file, line, column), expects[expect],
      Diagnostics::SYNTAX, Diagnostics::ERROR);

  diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
      Diagnostics::SelectionContext(
          Diagnostics::FileContext(file, line, column), '^',
          "Found '" + token + "' ( Type: " + deduce_type() + " ) instead",
          lexed_lines[file][line]),
      '~', token.length()));

  Diagnostics::push(diagnostic);
}

int32_t Token::assemble(std::vector<std::shared_ptr<Token>> & /*tokens*/,
                        const std::map<std::string, Symbol> & /*symbols*/,
                        uint16_t /*program_counter*/) {
  expected(Expected::INSTRUCTION_LABEL_OR_DIRECTIVE);
  return -1;
}

/*! All instructions take a specific number of arguments.
 *
 * For now, this handles the cases of too many arguments, as well as too few
 * arguments. In future, I plan to allow, pretty much all, instructions to be
 * put one after another, e.g.
 *
 * .ORIG 0x3000 ADD R1, R2, R3 AND R2, R4, R5 .END
 *
 * This is to give better compatability with the original assembler.
 *
 * @param provided The number of arguments provided for the instruction (+1,
 * explained below)
 * @param expected The number of expected arguments.
 * @param last_column The final column used for highlighting.
 */
void Token::invalid_argument_count(size_t provided, size_t expected,
                                   size_t last_column) const {
  // This is not the best idea, but because tokens.size() returns the number
  // of arguments + the token itself, it's a little easier to do here.
  --provided;

  std::stringstream error_string;
  error_string << token << " expects " << expected << " argument"
               << (expected == 1 ? "" : "'s") << ", but "
               << (provided < expected ? "only " : "") << provided
               << " argument" << (provided == 1 ? " was" : "'s were")
               << " provided";

  Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file, line, column), error_string.str(),
      Diagnostics::SYNTAX, Diagnostics::ERROR);

  if (0u != provided) { // To avoid printing the beginning token itself.
    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
        Diagnostics::SelectionContext(
            Diagnostics::FileContext(file, line, column + token.length()), ' ',
            "Unexpected arguments found here", lexed_lines[file][line]),
        '~', last_column - (column + token.length())));
  }

  Diagnostics::push(diagnostic);
}
