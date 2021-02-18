#ifndef OOPS_COMPILER_PARSER_FOR_H
#define OOPS_COMPILER_PARSER_FOR_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class for_statement : public statement, public parseable<for_statement> {
 private:
  std::unique_ptr<statement> declaration, body;
  std::unique_ptr<expression> conditional, increment;

 public:
  for_statement(std::unique_ptr<statement> declaration,
                std::unique_ptr<expression> conditional,
                std::unique_ptr<expression> increment,
                std::unique_ptr<statement> body)
      : declaration(std::move(declaration)),
        conditional(std::move(conditional)),
        increment(std::move(increment)),
        body(std::move(body)) {}
  const statement &get_declaration() { return *declaration; }
  const expression &get_conditional() { return *conditional; }
  const expression &get_increment() { return *increment; }
  const statement &get_body() { return *body; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif