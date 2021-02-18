#ifndef OOPS_COMPILER_PARSER_SEMICOLON_H
#define OOPS_COMPILER_PARSER_SEMICOLON_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class semicolon_statement : public statement, parseable<semicolon_statement> {
    private:
    std::unique_ptr<expression> expr;
    public:
    semicolon_statement(std::unique_ptr<expression> expr) : expr(std::move(expr)) {}
    const expression& get_expression() {
        return *expr;
    }
};
}  // namespace parser
}  // namespace oops_compiler

#endif