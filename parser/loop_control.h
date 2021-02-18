#ifndef OOPS_COMPILER_PARSER_LOOP_CONTROL_H
#define OOPS_COMPILER_PARSER_LOOP_CONTROL_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class break_statement : public statement, public parseable<break_statement> {
 private:
 public:
  break_statement() {}
};
class continue_statement : public statement, public parseable<continue_statement> {
 private:
 public:
  continue_statement() {}
};
}  // namespace parser
}  // namespace oops_compiler

#endif