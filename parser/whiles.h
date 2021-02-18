#ifndef OOPS_COMPILER_PARSER_WHILE_H
#define OOPS_COMPILER_PARSER_WHILE_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class while_statement : public statement, public parseable<while_statement> {
 private:
  std::unique_ptr<expression> conditional;
  std::unique_ptr<statement> block;

 public:
  while_statement(std::unique_ptr<expression> conditional,
                  std::unique_ptr<statement> block)
      : conditional(std::move(conditional)), block(std::move(block)) {}
  const expression &get_conditional() { return *conditional; }
  const statement &get_statement() { return *block; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif