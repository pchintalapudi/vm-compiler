#ifndef OOPS_COMPILER_PARSER_NODE_TYPE_H
#define OOPS_COMPILER_PARSER_NODE_TYPE_H

#include <tuple>

#include "base.h"
#include "class.h"
#include "expression.h"
#include "source_file.h"
#include "statement.h"
#include "type.h"

namespace oops_compiler {
namespace parser {
typedef std::tuple<identifier, class_variable, parameter, class_method,
                   class_definition, package_declaration, import_declaration,
                   source_file, basic_block, type_declaration,
                   type_instantiation, general_type, generic_bound>
    ast_node_types;
template <typename t, std::size_t... indices>
constexpr std::size_t index_of_ast_type(std::index_sequence<indices...>) {
  return ~0ull +
         ((std::is_same_v<t, std::tuple_element_t<indices, ast_node_types>>
               ? indices + 1
               : 0) +
          ...);
}

template <typename t>
constexpr std::size_t index_of_ast_type_v = index_of_ast_type<t>(
    std::make_index_sequence<std::tuple_size_v<ast_node_types>>());

template <typename self_t, typename base_t>
std::size_t visitable<self_t, base_t>::get_type_index() const {
  static_assert(index_of_ast_type_v<self_t> != ~0ull);
  return index_of_ast_type_v<self_t>;
}

template <typename func_t>
using ast_visitor = visitor<ast_node_types, func_t>;

template <typename visitor_t>
void ast_node::visit(visitor_t &visitor) {
  switch (this->get_type_index()) {
#define is(type)                                                      \
  case index_of_ast_type_v<type>:                                     \
    static_cast<type *>(this)->visit(visitor);                        \
    static_assert(__LINE__ - start == 1 + index_of_ast_type_v<type>); \
    break
    constexpr std::size_t start = __LINE__;
    is(identifier);
    is(class_variable);
    is(parameter);
    is(class_method);
    is(class_definition);
    is(package_declaration);
    is(import_declaration);
    is(source_file);
    is(basic_block);
    is(type_declaration);
    is(type_instantiation);
    is(general_type);
    is(generic_bound);
    static_assert(__LINE__ - start == std::tuple_size_v<ast_node_types> + 1);
#undef is
  }
}
}  // namespace parser
}  // namespace oops_compiler

#endif