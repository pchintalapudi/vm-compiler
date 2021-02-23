#ifndef OOPS_COMPILER_PARSER_ACCESS_EXPRESSION_H
#define OOPS_COMPILER_PARSER_ACCESS_EXPRESSION_H

#include "expression.h"

namespace oops_compiler {
namespace parser {

class access_chain : public expression {
 private:
  std::string identifier;
  std::unique_ptr<access_chain> chain;

 public:
  access_chain(std::string identifier, std::unique_ptr<access_chain> chain)
      : identifier(std::move(identifier)), chain(std::move(chain)) {}
  const std::string &get_identifier() const { return identifier; }
  const access_chain &get_chain() const { return *chain; }
};
class access_expression : public expression {
 private:
  std::unique_ptr<expression> root;
  std::optional<access_chain> chain;

 public:
  access_expression(decltype(root) root, decltype(chain) chain)
      : root(std::move(root)), chain(std::move(chain)) {}
  const expression &get_root() const { return *root; }

  const decltype(chain) &get_identifier() const { return chain; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif