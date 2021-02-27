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
                   type_instantiation, general_type>
    ast_node_types;
template <typename t, std::size_t... indices>
constexpr std::size_t index_of(std::index_sequence<indices...>) {
  return ~0ull +
         ((std::is_same_v<t, std::tuple_element_t<indices, ast_node_types>>
               ? indices + 1
               : 0) +
          ...);
}

template <typename self_t, typename base_t>
std::size_t visitable<self_t, base_t>::get_type_index() const {
  static_assert(
      index_of<self_t>(
          std::make_index_sequence<std::tuple_size_v<ast_node_types>>()) !=
      ~0ull);
  return index_of<self_t>(
      std::make_index_sequence<std::tuple_size_v<ast_node_types>>());
}

template <typename func_t>
using ast_visitor = visitor<ast_node_types, func_t>;
}  // namespace parser
}  // namespace oops_compiler

#endif