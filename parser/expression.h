#ifndef OOPS_COMPILER_PARSER_EXPRESSION_H
#define OOPS_COMPILER_PARSER_EXPRESSION_H

#include <memory>
#include <vector>

#include "../lexer/token.h"
#include "base_classes.h"
#include "scope.h"

namespace oops_compiler {
namespace parser {
class expression {
 private:
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

class indexed : public expression {
 private:
  std::unique_ptr<expression> subexpr;

 public:
  indexed(std::unique_ptr<expression> subexpr)
      : subexpr(std::move(subexpr)) {}

  const expression &subexpression() const { return *subexpr; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif