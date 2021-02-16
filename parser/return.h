#ifndef OOPS_COMPILER_PARSER_RETURN_H
#define OOPS_COMPILER_PARSER_RETURN_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class return_statement : public statement {
    private:
    public:
    const expression& get_expression();
    static std::pair<return_statement, std::size_t> parse(std::vector<lexer::token> tokens, std::size_t start);
};
}  // namespace parser
}  // namespace oops_compiler

#endif