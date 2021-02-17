#ifndef OOPS_COMPILER_PARSER_EXPRESSION_H
#define OOPS_COMPILER_PARSER_EXPRESSION_H

#include <vector>

#include "../lexer/token.h"

namespace oops_compiler {
namespace parser {
    class symbol_table;
class expression {
 private:
 public:
  virtual ~expression() = default;
};
}  // namespace parser
}  // namespace oops_compiler

#endif