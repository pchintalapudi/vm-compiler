#ifndef OOPS_COMPILER_PARSER_THROW_H
#define OOPS_COMPILER_PARSER_THROW_H

#include "expression.h"
#include "semicolon.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class throw_statement : public statement, public parseable<throw_statement> {
 private:
  semicolon_statement statement;

 public:
  throw_statement(semicolon_statement statement)
      : statement(std::move(statement)) {}
  const semicolon_statement &get_statement() { return statement; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif