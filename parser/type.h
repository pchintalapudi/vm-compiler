#ifndef OOPS_COMPILER_PARSER_TYPE_H
#define OOPS_COMPILER_PARSER_TYPE_H

#include <memory>
#include <vector>

#include "base.h"
#include "expression.h"

namespace oops_compiler {
namespace parser {
struct general_type;
struct type_instantiation;
struct generic_bound : virtual ast_node,
                       virtual visitable<generic_bound, ast_node> {
  std::unique_ptr<identifier> name;
  std::vector<std::unique_ptr<general_type>> lower_bounds, upper_bounds;
  generic_bound(decltype(name) name, decltype(lower_bounds) lower_bounds,
                decltype(upper_bounds) upper_bounds)
      : name(std::move(name)),
        lower_bounds(std::move(lower_bounds)),
        upper_bounds(std::move(upper_bounds)) {}
  using visitable<generic_bound, ast_node>::visit;
};
struct type_declaration : virtual ast_node,
                          virtual visitable<type_declaration, ast_node> {
  std::unique_ptr<identifier> name;
  std::vector<std::unique_ptr<generic_bound>> bounds;
  using visitable<type_declaration, ast_node>::visit;
  type_declaration(decltype(name) name, decltype(bounds) bounds)
      : name(std::move(name)), bounds(std::move(bounds)) {}
};
struct type_instantiation : virtual expression,
                            virtual visitable<type_instantiation, expression> {
  std::unique_ptr<identifier> name;
  std::vector<std::unique_ptr<general_type>> generics;
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