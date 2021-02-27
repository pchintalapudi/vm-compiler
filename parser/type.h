#ifndef OOPS_COMPILER_PARSER_TYPE_H
#define OOPS_COMPILER_PARSER_TYPE_H

#include <memory>
#include <vector>

#include "base.h"
#include "expression.h"

namespace oops_compiler {
namespace parser {
struct type_declaration : virtual ast_node,
                          virtual visitable<type_declaration, ast_node> {
  using visitable<type_declaration, ast_node>::visit;
};
struct type_instantiation : virtual expression,
                            virtual visitable<type_instantiation, expression> {
  std::unique_ptr<identifier> name;
  std::vector<std::unique_ptr<type_instantiation>> generics;
  type_instantiation(decltype(name) name, decltype(generics) generics)
      : name(std::move(name)), generics(std::move(generics)) {}
  using visitable<type_instantiation, expression>::visit;
};
struct general_type : virtual expression,
                      virtual visitable<general_type, expression> {
  using visitable<general_type, expression>::visit;
};
}  // namespace parser
}  // namespace oops_compiler

#endif