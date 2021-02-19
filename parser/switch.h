#ifndef OOPS_COMPILER_PARSER_SWITCH_H
#define OOPS_COMPILER_PARSER_SWITCH_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class case_statement : public statement {
 private:
  std::unique_ptr<statement> substatement;
  lexer::token literal;

 public:
  case_statement(std::unique_ptr<statement> substatement, lexer::token literal)
      : substatement(std::move(substatement)), literal(std::move(literal)) {}
  const statement &get_substatement() const { return *substatement; }
  const lexer::token &get_literal() const { return literal; }
};
class default_statement {
 private:
  std::unique_ptr<statement> substatement;

 public:
  default_statement(std::unique_ptr<statement> substatement)
      : substatement(std::move(substatement)) {}
  const statement &get_substatement() const { return *substatement; }
};
class switch_statement : public statement {
 private:
  std::vector<case_statement> cases;
  std::optional<default_statement> defaulted;

 public:
  switch_statement(std::vector<case_statement> cases,
                   std::optional<default_statement> defaulted)
      : cases(std::move(cases)), defaulted(std::move(defaulted)) {}
  const std::vector<case_statement> &get_cases() const;
  const std::optional<default_statement> &get_default() const {
    return defaulted;
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif