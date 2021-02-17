#ifndef OOPS_COMPILER_PARSER_EXPRESSION_H
#define OOPS_COMPILER_PARSER_EXPRESSION_H

#include <vector>

#include "../lexer/token.h"
#include "symbol_table.h"

namespace oops_compiler {
namespace parser {
class expression {
 private:
 public:
  virtual ~expression() = default;
  static std::pair<expression, std::size_t> parse(
      const std::vector<lexer::token> &tokens, std::size_t start,
      symbol_table &symbols);
  static std::pair<expression, std::size_t> parse_parenthetical(
      const std::vector<lexer::token> &tokens, std::size_t start,
      symbol_table &symbols);
  static std::pair<expression, std::size_t> parse_indexer(
      const std::vector<lexer::token> &tokens, std::size_t start,
      symbol_table &symbols);
};
}  // namespace parser
}  // namespace oops_compiler

#endif