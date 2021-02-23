#ifndef OOPS_COMPILER_PARSER_EXPRESSION_H
#define OOPS_COMPILER_PARSER_EXPRESSION_H

#include <memory>
#include <vector>

#include "../lexer/token.h"
#include "base_classes.h"

namespace oops_compiler {
namespace parser {
class expression {
 public:
  virtual ~expression() = default;
};

class parenthetical : public expression {
 private:
  std::unique_ptr<expression> subexpr;

 public:
  parenthetical(std::unique_ptr<expression> subexpr)
      : subexpr(std::move(subexpr)) {}

  const expression &subexpression() const { return *subexpr; }
};

class literal_expression : public expression {
 private:
  lexer::token lit;

 public:
  literal_expression(lexer::token lit) : lit(std::move(lit)) {}
  const lexer::token &get_literal() const { return lit; }
};

class identifier_expression : public expression {
 private:
  const char *start;
  std::size_t size;

 public:
  identifier_expression(const char *start, std::size_t size)
      : start(start), size(size) {}

  const char *get_start() const { return start; }
  std::size_t get_size() const { return size; }
};

class empty_expression : public expression {
 private:
 public:
};
}  // namespace parser
}  // namespace oops_compiler

#endif