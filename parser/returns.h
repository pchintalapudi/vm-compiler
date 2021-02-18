#ifndef OOPS_COMPILER_PARSER_RETURN_H
#define OOPS_COMPILER_PARSER_RETURN_H

#include "expression.h"
#include "statement.h"
#include "semicolon.h"

namespace oops_compiler {
namespace parser {
class return_statement : public statement, public parseable<return_statement> {
    private:
    semicolon_statement statement;
    public:
    return_statement(semicolon_statement statement) : statement(std::move(statement)) {}
    const semicolon_statement& get_statement();
};
}  // namespace parser
}  // namespace oops_compiler

#endif