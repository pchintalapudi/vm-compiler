#ifndef OOPS_COMPILER_PARSER_SWITCH_H
#define OOPS_COMPILER_PARSER_SWITCH_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class case_statement : public statement {
 private:
  statement substatement;

 public:
  case_statement(statement substatement) : substatement(substatement) {}
  const statement &get_substatement() const;
  static std::pair<case_statement, std::size_t> parse(
      const std::vector<lexer::token> &tokens, std::size_t start,
      symbol_table &symbols);
};
class default_statement : public case_statement {
 public:
  default_statement(statement substatement) : case_statement(substatement) {}
  const statement &get_substatement() const;
  static std::pair<default_statement, std::size_t> parse(
      const std::vector<lexer::token> &tokens, std::size_t start,
      symbol_table &symbols);
};
class switch_statement : public statement {
 private:
  std::vector<case_statement> cases;

 public:
  switch_statement(std::vector<case_statement> cases) : cases(cases) {}
  const std::vector<case_statement> &get_cases() const;
  static std::pair<switch_statement, std::size_t> parse(
      const std::vector<lexer::token> &tokens, std::size_t start,
      symbol_table &symbols);
};
}  // namespace parser
}  // namespace oops_compiler

#endif