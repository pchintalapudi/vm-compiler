#ifndef OOPS_COMPILER_PARSER_ACCESS_EXPRESSION_H
#define OOPS_COMPILER_PARSER_ACCESS_EXPRESSION_H

#include "expression.h"

namespace oops_compiler {
namespace parser {
class access_expression : public expression {
 private:
  std::unique_ptr<expression> root;
  std::string identifier;

 public:
  const expression &get_root() const { return *root; }

  const std::string &get_identifier() const { return identifier; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif