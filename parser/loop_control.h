#ifndef OOPS_COMPILER_PARSER_LOOP_CONTROL_H
#define OOPS_COMPILER_PARSER_LOOP_CONTROL_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class break_statement : public statement {
 private:
 public:
  break_statement() {}
  static std::pair<break_statement, std::size_t> parse(
      const std::vector<lexer::token> &tokens, std::size_t start,
      symbol_table &symbols);
};
class continue_statement : public statement {
 private:
 public:
  continue_statement() {}
  static std::pair<continue_statement, std::size_t> parse(
      const std::vector<lexer::token> &tokens, std::size_t start,
      symbol_table &symbols);
};
}  // namespace parser
}  // namespace oops_compiler

#endif