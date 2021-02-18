#ifndef OOPS_COMPILER_PARSER_ENHANCED_FOR_H
#define OOPS_COMPILER_PARSER_ENHANCED_FOR_H

#include "base_classes.h"
#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class enhanced_for_statement : public statement,
                               public parseable<enhanced_for_statement> {
 private:
  std::unique_ptr<statement> declaration;
  std::unique_ptr<expression> iterable;
  std::unique_ptr<statement> body;

 public:
  enhanced_for_statement(std::unique_ptr<statement> declaration,
                         std::unique_ptr<expression> iterable,
                         std::unique_ptr<statement> body)
      : declaration(std::move(declaration)),
        iterable(std::move(iterable)),
        body(std::move(body)) {}
  const statement &get_declaration() { return *declaration; }
  const expression &get_iterable() { return *iterable; }
  const statement &get_body() { return *body; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif