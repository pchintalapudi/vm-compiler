#ifndef OOPS_COMPILER_PARSER_DECLARATION_H
#define OOPS_COMPILER_PARSER_DECLARATION_H

#include <string>

#include "access_expression.h"
#include "base_classes.h"
#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class declaration : public statement {
 private:
  std::variant<type_instantiation, access_expression> type;
  std::string name;
  std::unique_ptr<expression> expr;

 public:
  declaration(decltype(declaration::type) type, std::string name,
              std::optional<std::unique_ptr<expression>> expr)
      : type(std::move(type)), name(std::move(name)), expr(std::move(*expr)) {}
  const decltype(declaration::type) &get_type() const { return type; }
  const std::string &variable_name() const { return name; }
  std::optional<const expression *> get_expression() const {
    if (expr) {
      return expr.get();
    } else {
      return {};
    }
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif