#include <sstream>

#include "catch.hpp"

#include "Diagnostics.hpp"
#include "Lexer.hpp"
#include "Tokens/Tokens.hpp"
#include "iomanip"

TEST_CASE(
  "The lexer can lex any 16 bit decimal, hexadecimal, binary, and octal value",
  "[lexer]")
{
  const std::string f_name = "TEST";

  for (int i = std::numeric_limits<int16_t>::min();
       i < std::numeric_limits<int16_t>::max() + 1;
       ++i) {
    std::stringstream ss;
    ss << "#" << i;

    auto tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Decimal>(tokens.front())->value == i);

    ss.str("");
    ss << i;
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Decimal>(tokens.front())->value == i);

    ss.str("");
    ss << "0x" << std::hex << static_cast<int16_t>(i);
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Hexadecimal>(tokens.front())->value == i);

    ss.str("");
    ss << 'x' << std::hex << static_cast<int16_t>(i);
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Hexadecimal>(tokens.front())->value ==
            static_cast<int16_t>(i));

    ss.str("");
    ss << "0X" << std::hex << static_cast<int16_t>(i);
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Hexadecimal>(tokens.front())->value == i);

    ss.str("");
    ss << 'X' << std::hex << static_cast<int16_t>(i);
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Hexadecimal>(tokens.front())->value == i);

    ss.str("");
    ss << "0b" << std::bitset<16>(static_cast<int16_t>(i));
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Hexadecimal>(tokens.front())->value == i);

    ss.str("");
    ss << 'b' << std::bitset<16>(static_cast<int16_t>(i));
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Hexadecimal>(tokens.front())->value ==
            static_cast<int16_t>(i));

    ss.str("");
    ss << "0B" << std::bitset<16>(static_cast<int16_t>(i));
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Hexadecimal>(tokens.front())->value == i);

    ss.str("");
    ss << 'B' << std::bitset<16>(static_cast<int16_t>(i));
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Hexadecimal>(tokens.front())->value == i);

#ifdef INCLUDE_ADDONS
    ss.str("");
    ss << '0' << std::oct << static_cast<int16_t>(i);
    tokens = Lexer::tokenize_line(Line(ss.str()), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::IMMEDIATE);
    REQUIRE(std::static_pointer_cast<Octal>(tokens.front())->value == i);
#endif

    REQUIRE(Diagnostics::count() == 0);
  }
}

std::vector<std::string>
permutations(std::string str)
{
  std::vector<std::string> perms;

  const size_t bitmap = 2 << (str.length() - 1);

  for (size_t i = 0; i < bitmap; ++i) {
    std::string s;
    for (size_t j = 0; j < str.length(); ++j) {
      if (0 != (i & (1 << j))) {
        s += std::toupper(str[j]);
      } else {
        s += std::tolower(str[j]);
      }
    }

    perms.push_back(s);
  }

  return perms;
}

TEST_CASE("The lexer should be able to properly tokenize any variation of an "
          "instruction, directive, or register",
          "[lexer]")
{
  const std::string f_name = "TEST";

  for (auto i : permutations("add")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_ADD);
  }

  for (auto i : permutations("and")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_AND);
  }

  for (auto i : permutations("not")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_NOT);
  }

  for (auto i : permutations("br")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_BR);
  }

  for (auto i : permutations("not")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_NOT);
  }

  for (auto i : permutations("jmp")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_JMP);
  }

  for (auto i : permutations("jsr")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_JSR);
  }

  for (auto i : permutations("jsrr")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_JSRR);
  }

  for (auto i : permutations("ret")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_RET);
  }

  for (auto i : permutations("rti")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_RTI);
  }

  for (auto i : permutations("ld")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_LD);
  }

  for (auto i : permutations("ldi")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_LDI);
  }

  for (auto i : permutations("ldr")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_LDR);
  }

  for (auto i : permutations("lea")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_LEA);
  }

  for (auto i : permutations("st")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_ST);
  }

  for (auto i : permutations("sti")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_STI);
  }

  for (auto i : permutations("str")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::OP_STR);
  }

#ifdef INCLUDE_ADDONS
  for (auto i : permutations(".lshift")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::ADDON_LSHIFT);
  }

  for (auto i : permutations(".neg")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::ADDON_NEG);
  }

  for (auto i : permutations(".sub")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::ADDON_SUB);
  }

  for (auto i : permutations(".set")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::ADDON_SET);
  }

  for (auto i : permutations(".include")) {
    auto tokens = Lexer::tokenize_line(Line(i), f_name, 0);
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.front()->type() == Token::ADDON_INCLUDE);
  }
#endif

  REQUIRE(Diagnostics::count() == 0);
}