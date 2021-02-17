#ifndef OOPS_COMPILER_PARSER_RETURN_H
#define OOPS_COMPILER_PARSER_RETURN_H

#include "expression.h"
#include "statement.h"
#include "semicolon.h"

namespace oops_compiler {
namespace parser {
class return_statement : public statement {
    private:
    semicolon_statement statement;
    public:
    return_statement(semicolon_statement statement) : statement(statement) {}
    const semicolon_statement& get_statement();
    static std::pair<return_statement, std::size_t> parse(const std::vector<lexer::token> &tokens, std::size_t start, symbol_table &symbols);
};
}  // namespace parser
}  // namespace oops_compiler

#endif