#ifndef OOPS_COMPILER_PARSER_EXPRESSION_H
#define OOPS_COMPILER_PARSER_EXPRESSION_H

#include <vector>

#include "../lexer/token.h"

namespace oops_compiler {
namespace parser {
class expression {
 private:
 public:
 virtual ~expression();
 static std::pair<expression, std::size_t> parse(std::vector<lexer::token> tokens, std::size_t start);
 static std::pair<expression, std::size_t> parse_parenthetical(std::vector<lexer::token> tokens, std::size_t start);
 static std::pair<expression, std::size_t> parse_indexer(std::vector<lexer::token> tokens, std::size_t start);
};
}  // namespace parser
}  // namespace oops_compiler

#endif