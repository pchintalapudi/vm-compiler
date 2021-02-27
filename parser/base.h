#ifndef OOPS_COMPILER_PARSER_BASE_H
#define OOPS_COMPILER_PARSER_BASE_H

#include <type_traits>

#include "../logger/logging.h"

namespace oops_compiler {
namespace parser {
struct ast_node {
  virtual std::size_t get_type_index() const = 0;
  template <typename visitor_t>
  void visit(visitor_t &visitor);
  virtual ~ast_node() = default;
};

template <typename tuple_t, typename func_t>
struct visitor {
  func_t apply;
  template <typename node_t, std::size_t... indices>
  static constexpr bool visitable(std::index_sequence<indices...>) {
    return (
        (std::is_base_of_v<std::tuple_element_t<indices, tuple_t>, node_t>) ||
        ...);
  }
  template <typename node_t>
  std::enable_if_t<
      visitable<node_t>(std::make_index_sequence<std::tuple_size_v<tuple_t>>()),
      void>
  operator()(node_t &node) {
    apply(node);
  }
};
template <typename self_t, typename base_t>
struct visitable : virtual base_t {
  virtual std::size_t get_type_index() const override;
  template <typename visitor_t>
  void visit(visitor_t &visitor) {
    visitor(static_cast<self_t &>(*this));
  }
};

struct identifier : virtual ast_node, virtual visitable<identifier, ast_node> {
  const char *start;
  std::size_t size;
  logger::context context;
  identifier(decltype(start) start, decltype(size) size,
             decltype(context) context)
      : start(start), size(size), context(std::move(context)) {}

  std::string str() const { return std::string{start, size}; }
  using visitable<identifier, ast_node>::visit;
};
}  // namespace parser
}  // namespace oops_compiler

#endif