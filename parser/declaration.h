#ifndef OOPS_COMPILER_PARSER_DECLARATION_H
#define OOPS_COMPILER_PARSER_DECLARATION_H

#include <string>

#include "base_classes.h"
#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class declaration : public statement, parseable<declaration> {
 private:
  std::string name;
  std::unique_ptr<expression> expr;

 public:
  declaration(std::string name, std::unique_ptr<expression> expr)
      : name(std::move(name)), expr(std::move(expr)) {}
  const std::string &variable_name() { return name; }
  const expression &get_expression() { return *expr; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif