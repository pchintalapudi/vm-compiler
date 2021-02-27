#ifndef OOPS_COMPILER_PARSER_BASE_H
#define OOPS_COMPILER_PARSER_BASE_H

#include "../logger/logging.h"

namespace oops_compiler {
namespace parser {
struct ast_node {
  virtual ~ast_node() = default;
};
struct identifier : ast_node {
  const char *start;
  std::size_t size;
  logger::context context;
  identifier(decltype(start) start, decltype(size) size,
             decltype(context) context)
      : start(start), size(size), context(std::move(context)) {}

  std::string str() const { return std::string{start, size}; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif