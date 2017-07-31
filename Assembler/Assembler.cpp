#include "Assembler.hpp"

#include <fstream>
#include <iomanip>

#include "Configuration.hpp"
#include "Diagnostics.hpp"
#include "LexHelper.hpp"
#include "Parser.hpp"
#include "cxxopts.hpp"

int
Assembler::assemble(int argc, char** args)
{
  // Won't be using C-style input/output, so why not.
  std::ios_base::sync_with_stdio(false);

  Assembler assembler;
  if (!assembler.configure(argc, args)) {
    return 1;
  }

  assembler.assemble();

  return 0;
}

/*! Configure the assembler
 *
 * @param argc The argument count
 * @param args Command line arguments
 * @return Whether configuration succeeded or not.
 */
bool
Assembler::Assembler::configure(int argc, char** args)
{
  cxxopts::Options option_parser("LC3AS", "LC-3 Assembly Language Assembler");
  try {
    option_parser.positional_help("<assembly files>");
    option_parser.add_options()("h,help", "Print this help message")(
      "I,include",
      "Add a directory to search for .INCLUDE directives",
      cxxopts::value<std::vector<std::string>>(),
      "DIRECTORY")("s,stop", "Stop on the first error")(
      "e,error", "Report warnings as errors")(
      "w,warn", "Choose warning type <Not implemented>") // TODO: Fix this
      ("files",
       "Files to assemble",
       cxxopts::value<std::vector<std::string>>())("q,quiet", "Be quiet")(
        "keep-going",
        "Keep going despite errors (mostly used for testing). Doesn't write "
        "to files unless no errors occurred)")(
        "no-colour", "Disable coloured output (synonymous to no-color)");

    // TODO: Add a --attempt-fix. Basically, assume that a problem is meant
    // TODO: to be fixed to what we think it should, e.g. when we encounter a
    // TODO: single '/', the assembler keeps going because it assumes that it
    // TODO: was meant to be a double '//', indication a comment. Do the same
    // TODO: thing for character literals and strings -- basically, anything
    // TODO: left on that line will be treated as a string/character (which, of
    // TODO: course, would lead to problems with character literals).
    // TODO:    - Maybe only use 1 character for character literals, and
    // TODO: anything else as extra?

    option_parser.parse_positional("files");
    option_parser.parse(argc, args);
  } catch (const cxxopts::OptionException& e) {
    std::cout << "LC3AS Command Line Error: " << e.what() << '\n';
    return false;
  }

  Config::add_search_directory(".", "Current Running Directory");

  if (0 != option_parser.count("help")) {
    std::cout << option_parser.help() << '\n';
    return false;
  }

  if (0 == option_parser.count("files")) {
    std::cout << "LC3AS: No input files.\n";
    return false;
  }

  const auto& files = option_parser["files"].as<std::vector<std::string>>();
  for (const auto& file : files) {
    files_to_assemble.emplace_back(file);
  }

  if (0 != option_parser.count("include")) {
    const auto& dirs = option_parser["include"].as<std::vector<std::string>>();
    for (const auto& dir : dirs) {
      Config::add_search_directory(dir);
    }
  }

  if (0 != option_parser.count("stop")) {
    Config::set(Config::CONFIG_OPTIONS::STOP_ON_FIRST_ERR);
  }

  if (0 != option_parser.count("error")) {
    Config::set(Config::CONFIG_OPTIONS::WARN_AS_ERROR);
  }

  if (0 != option_parser.count("warn")) {
    // TODO: Implement (as above).
  }

  if (0 != option_parser.count("quiet")) {
    Config::set(Config::CONFIG_OPTIONS::BE_QUIET);
  }

  if (0 != option_parser.count("keep-going")) {
    Config::set(Config::KEEP_GOING);
  }

  if (0 != option_parser.count("no-colour") ||
      0 != option_parser.count("no-color")) {
    Config::set(Config::NO_COLOUR);
  }

  return true;
}

/*! Assemble every file provided via the command line.
 *
 */
void
Assembler::Assembler::assemble()
{
  for (auto& file : files_to_assemble) {
    std::ifstream f{ file };

    if (f.fail()) {
      // We can't do anything with a file that failed to open for some
      // reason...
      Console::write_line(Diagnostics::Variant<std::string>(
        "Unable to open file '" + file + "'"));
      f.close();
      continue;
    }

    f.close();

    if (!Config::is_set(Config::CONFIG_OPTIONS::BE_QUIET)) {
      Console::write(Diagnostics::Variant<std::string>(
        "--- ", Console::Colour(Console::FOREGROUND_COLOUR::RESET)));
      Console::write(Diagnostics::Variant<std::string>(
        "Assembling file '" + file + "'",
        Console::Colour(Console::FOREGROUND_COLOUR::RESET)));
      Console::write(Diagnostics::Variant<std::string>(
        " ---\n", Console::Colour(Console::FOREGROUND_COLOUR::RESET)));
    }

    Parser parser(file);
    if ((0 == parser.parse()) || Config::is_set(Config::KEEP_GOING)) {
      // No errors (or user wants us to proceed), so let's write to the
      // associated files.
      tokens = parser.parsed_tokens();
      symbols = parser.parsed_symbols();

      symbol_padding = parser.symbol_padding();

      generate_machine_code();

      if (!Diagnostics::critical()) {
        // Only ever write if there were no errors (even if the user passed
        // --keep-going)
        write(file);
      }

      assembled.clear();
    }

    if (!Config::is_set(Config::CONFIG_OPTIONS::BE_QUIET)) {
      Diagnostics::unwind();
    } else {
      Diagnostics::reset();
    }
  }
}

/*! Generate a vector containing the assembled version of the code.
 *
 */
void
Assembler::Assembler::generate_machine_code()
{
  for (size_t i = 0; i < tokens.size(); ++i) {
    const auto& tokenized_line{ tokens[i] };

    for (const auto& assembled_line : tokenized_line.front()->as_assembled()) {
      if (!Config::is_set(Config::CONFIG_OPTIONS::BE_QUIET)) {
        // Not really much point in checking for these if the user wants
        // us to be quiet.
        check_and_mark_warnings(tokenized_line, assembled_line, i);
      }
      assembled.push_back(assembled_line);
    }
  }
}

/*! Perform last minute warning checks
 *
 * These checks are mostly for figuring out any small logic errors (if any)
 * there are in the code, e.g. A branch with an offset of 0 is superfluous, a
 * branch with an offset of -1 is likely an infinite loop, etc.
 *
 * Some of these are just there in the case they're accidents, as some of them
 * can be useful.
 *
 * @param tokenized_line The line we're working with
 * @param assembled_line The assembled version of the token
 * @param i Index into tokens which allows us to work with previous instructions
 */
void
Assembler::Assembler::check_and_mark_warnings(
  const std::vector<std::shared_ptr<Token>>& tokenized_line,
  uint16_t assembled_line,
  size_t i)
{
  // TODO: Move this to it's own place, and add more checks.

  // TODO: Add a .BLKW warning for taking up a lot of memory.

  if (tokenized_line.front()->type() == Token::OP_BR ||
      tokenized_line.front()->type() == Token::OP_JSR) {
    if (tokenized_line.front()->type() == Token::OP_BR &&
        ((assembled_line & 0xFE00) == (assembled.back() & 0xFE00))) {
      Diagnostics::Diagnostic diagnostic(
        Diagnostics::FileContext(tokenized_line.front()->file,
                                 tokenized_line.front()->line,
                                 tokenized_line.front()->column),
        "Statement before this one checks for the same condition code.",
        Diagnostics::LOGIC,
        Diagnostics::WARNING);

      diagnostic.provide_context(
        std::make_unique<Diagnostics::HighlightContext>(
          Diagnostics::SelectionContext(
            Diagnostics::FileContext(tokenized_line.front()->file,
                                     tokenized_line.front()->line,
                                     tokenized_line.front()->column),
            '^',
            "This might mean this line is superfluous",
            lexed_lines[tokenized_line.front()->file].at(
              tokenized_line.front()->line)),
          '~',
          tokenized_line.front()->token.length()));

      diagnostic.provide_context(
        std::make_unique<Diagnostics::HighlightContext>(
          Diagnostics::SelectionContext(
            Diagnostics::FileContext(tokens[i - 1].front()->file,
                                     tokens[i - 1].front()->line,
                                     tokens[i - 1].front()->column),
            '^',
            "Checks the same condition code as this line",
            lexed_lines[tokens[i - 1].front()->file].at(
              tokens[i - 1].front()->line)),
          '~',
          tokens[i - 1].front()->token.length()));

      Diagnostics::push(diagnostic);
    }

    if (0 == (assembled_line & 0x1FF)) {
      Diagnostics::Diagnostic diagnostic(
        Diagnostics::FileContext(tokenized_line.front()->file,
                                 tokenized_line.front()->line,
                                 tokenized_line.front()->column),
        "Superfluous statement",
        Diagnostics::LOGIC,
        Diagnostics::WARNING);

      diagnostic.provide_context(
        std::make_unique<Diagnostics::HighlightContext>(
          Diagnostics::SelectionContext(
            Diagnostics::FileContext(tokenized_line[1]->file,
                                     tokenized_line[1]->line,
                                     tokenized_line[1]->column),
            '^',
            "Offset of 0 is superfluous -- simulator will go to "
            "next line no matter what",
            lexed_lines[tokenized_line[1]->file].at(tokenized_line[1]->line)),
          '~',
          tokenized_line[1]->token.length()));

      if (tokenized_line[1]->type() == Token::LABEL) {
        auto&& sym = symbols.find(tokenized_line[1]->token);
        diagnostic.provide_context(
          std::make_unique<Diagnostics::HighlightContext>(
            Diagnostics::SelectionContext(
              Diagnostics::FileContext(
                sym->second.file, sym->second.line_number, sym->second.column),
              '^',
              "Referred to label defined here",
              lexed_lines[sym->second.file].at(sym->second.line_number)),
            '~',
            sym->first.length()));
      }

      Diagnostics::push(diagnostic);
    } else if ((assembled_line & 0x1FF) == 0x1FF) {
      Diagnostics::Diagnostic diagnostic(
        Diagnostics::FileContext(tokenized_line.front()->file,
                                 tokenized_line.front()->line,
                                 tokenized_line.front()->column),
        "Possible infinite loop",
        Diagnostics::LOGIC,
        Diagnostics::WARNING);

      diagnostic.provide_context(
        std::make_unique<Diagnostics::HighlightContext>(
          Diagnostics::SelectionContext(
            Diagnostics::FileContext(tokenized_line[1]->file,
                                     tokenized_line[1]->line,
                                     tokenized_line[1]->column),
            '^',
            "Offset of -1 might cause an infinite loop",
            lexed_lines[tokenized_line[1]->file].at(tokenized_line[1]->line)),
          '~',
          tokenized_line[1]->token.length()));

      if (tokenized_line[1]->type() == Token::LABEL) {
        auto&& sym = symbols.find(tokenized_line[1]->token);
        diagnostic.provide_context(
          std::make_unique<Diagnostics::HighlightContext>(
            Diagnostics::SelectionContext(
              Diagnostics::FileContext(
                sym->second.file, sym->second.line_number, sym->second.column),
              '^',
              "Referred to label defined here",
              lexed_lines[sym->second.file].at(sym->second.line_number)),
            '~',
            sym->first.length()));
      }

      Diagnostics::push(diagnostic);
    }
  } else if (tokenized_line.front()->type() == Token::OP_TRAP) {
    if ((assembled_line & 0x00FF) > 0x0025 ||
        (assembled_line & 0x00FF) < 0x0020) {
      Diagnostics::Diagnostic diagnostic(
        Diagnostics::FileContext(tokenized_line.front()->file,
                                 tokenized_line.front()->line,
                                 tokenized_line.front()->column),
        "Possible illegal trap vector",
        Diagnostics::LOGIC,
        Diagnostics::WARNING);

      diagnostic.provide_context(
        std::make_unique<Diagnostics::HighlightContext>(
          Diagnostics::SelectionContext(
            Diagnostics::FileContext(tokenized_line[1]->file,
                                     tokenized_line[1]->line,
                                     tokenized_line[1]->column),
            '^',
            "Trap vector not between 0x20 and 0x25 (inclusive) might "
            "cause an illegal trap vector exception",
            lexed_lines[tokenized_line[1]->file].at(tokenized_line[1]->line)),
          '~',
          tokenized_line[1]->token.length(),
          "0x2{0..5}"));

      Diagnostics::push(diagnostic);
    }
  }
}

/*!
 * Write the instructions to all relevant files.
 *
 * symbol file (.sym): The symbols found in the file.
 * hexadecimal file (.hex): The hexadecimal representation of each instruction.
 * binary file (.bin): The binary representation of each instruction.
 * object file (.obj): The machine code of each instruction.
 * list file (.lst):   Each instruction with it's address, hex representation,
 * binary representation, line number, and the string representation.
 *
 * @param file The source code file name as a string
 */
void
Assembler::Assembler::write(const std::string& file)
{
  auto&& prefix = file.substr(0, file.find_last_of('.'));

  std::ofstream object_file(prefix + ".obj", std::ofstream::binary);
  std::ofstream binary_file(prefix + ".bin");
  std::ofstream symbol_file(prefix + ".sym");
  std::ofstream hex_file(prefix + ".hex");
  std::ofstream lst_file(prefix + ".lst");

  for (const auto instruction : assembled) {
    object_file.put(static_cast<char>((instruction >> 8) & 0xFF));
    object_file.put(static_cast<char>(instruction & 0xFF));

    binary_file << std::bitset<16>(instruction).to_string() << '\n';

    hex_file << std::setfill('0') << std::uppercase << std::hex << std::setw(4)
             << instruction << '\n';
  }

  symbol_file << "// Symbol table\n"
              << "// Scope Level 0:\n"
              << "//\t" << std::left << std::setfill(' ')
              << std::setw(symbol_padding) << "Symbol Name"
              << " Page Address\n//\t" << std::setfill('-')
              << std::setw(symbol_padding) << '-' << " ------------\n";

  for (const auto& symbol : symbols) {
    symbol_file << "//\t" << std::setfill(' ') << std::setw(symbol_padding)
                << symbol.first << ' ' << std::uppercase << std::hex
                << std::setfill('0') << std::setw(4) << symbol.second.address
                << '\n';
  }

  const auto& symbol_at = [this](const std::uint16_t address) {
    return std::find_if(
      symbols.cbegin(), symbols.cend(), [address](const auto& sym) -> bool {
        return sym.second.address == address;
      });
  };

  std::uint16_t pc = 0;

  const std::string empty;

  for (const auto& tokenized_line : tokens) {
    const auto& symbol = symbol_at(pc);

    // TODO: Should the file name be removed from the lst file?
    lst_file << tokenized_line.front()->disassemble(
      pc, symbol == symbols.cend() ? empty : symbol->first, symbol_padding);

    if (tokenized_line.front()->type() == Token::DIR_END) {
      // Ignore anything after the .END statement.
      break;
    }
  }
}
