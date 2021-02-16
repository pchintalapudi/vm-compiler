#ifndef OOPS_COMPILER_PARSER_WHILE_H
#define OOPS_COMPILER_PARSER_WHILE_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class while_statement : public statement {
    private:
    expression conditional;
    statement block;
    public:
    while_statement(expression conditional, statement block) : conditional(conditional), block(block) {}
    const expression& get_conditional();
    const statement& get_statement();
    static std::pair<while_statement, std::size_t> parse(std::vector<lexer::token> tokens, std::size_t start);
};
}  // namespace parser
}  // namespace oops_compiler

#endif