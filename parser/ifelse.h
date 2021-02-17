#ifndef OOPS_COMPILER_PARSER_IF_H
#define OOPS_COMPILER_PARSER_IF_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class if_statement : public statement {
    private:
    expression conditional;
    statement block;
    public:
    if_statement(expression conditional, statement block) : conditional(conditional), block(block) {}
    const expression& get_conditional();
    const statement& get_statement();
};
class else_statement : public statement {
    private:
    statement block;
    public:
    else_statement(statement block) : block(block) {}
    const expression& get_conditional();
    const statement& get_statement();
};
}  // namespace parser
}  // namespace oops_compiler

#endif