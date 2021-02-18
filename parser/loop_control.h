#ifndef OOPS_COMPILER_PARSER_LOOP_CONTROL_H
#define OOPS_COMPILER_PARSER_LOOP_CONTROL_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class break_expression : public expression {
 private:
 public:
  break_expression() {}
};
class continue_expression : public expression {
 private:
 public:
  continue_expression() {}
};
}  // namespace parser
}  // namespace oops_compiler

#endif