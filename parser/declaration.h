#ifndef OOPS_COMPILER_PARSER_DECLARATION_H
#define OOPS_COMPILER_PARSER_DECLARATION_H

#include <string>

#include "base_classes.h"
#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class declaration : public statement {
 private:
  const type_instantiation *type;
  std::string name;
  std::unique_ptr<expression> expr;

 public:
  declaration(const type_instantiation &type, std::string name,
              std::unique_ptr<expression> expr)
      : type(&type), name(std::move(name)), expr(std::move(expr)) {}
  const type_instantiation &get_type() const;
  const std::string &variable_name() const { return name; }
  const expression &get_expression() const { return *expr; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif