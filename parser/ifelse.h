#ifndef OOPS_COMPILER_PARSER_IF_H
#define OOPS_COMPILER_PARSER_IF_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class if_statement : public statement {
 private:
  std::unique_ptr<expression> conditional;
  std::unique_ptr<statement> block;

 public:
  if_statement(std::unique_ptr<expression> conditional,
               std::unique_ptr<statement> block)
      : conditional(std::move(conditional)), block(std::move(block)) {}
  const expression &get_conditional() { return *conditional; }
  const statement &get_statement() { return *block; }
};
class else_statement : public statement {
 private:
  std::unique_ptr<statement> block;

 public:
  else_statement(std::unique_ptr<statement> block) : block(std::move(block)) {}
  const statement &get_statement() { return *block; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif