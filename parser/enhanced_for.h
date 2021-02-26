#ifndef OOPS_COMPILER_PARSER_ENHANCED_FOR_H
#define OOPS_COMPILER_PARSER_ENHANCED_FOR_H

#include "base_classes.h"
#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class enhanced_for_statement : public statement {
 private:
  std::unique_ptr<general_type> type;
  std::string variable;
  std::unique_ptr<expression> iterable;
  std::unique_ptr<statement> body;

 public:
  enhanced_for_statement(decltype(type) type, decltype(variable) variable,
                         std::unique_ptr<expression> iterable,
                         std::unique_ptr<statement> body)
      : type(std::move(type)),
        variable(std::move(variable)),
        iterable(std::move(iterable)),
        body(std::move(body)) {}
  const general_type &get_type() { return *type; }
  const decltype(variable) get_variable() { return variable; }
  const expression &get_iterable() { return *iterable; }
  const statement &get_body() { return *body; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif