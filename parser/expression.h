#ifndef OOPS_COMPILER_PARSER_EXPRESSION_H
#define OOPS_COMPILER_PARSER_EXPRESSION_H

#include <memory>
#include <vector>

#include "../lexer/token.h"
#include "base_classes.h"
#include "scope.h"

namespace oops_compiler {
namespace parser {
class expression : public parseable<expression> {
 private:
 public:
  virtual ~expression() = default;
};
}  // namespace parser
}  // namespace oops_compiler

#endif