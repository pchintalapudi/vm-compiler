#ifndef OOPS_COMPILER_PARSER_THROW_H
#define OOPS_COMPILER_PARSER_THROW_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class throw_statement : public statement {
    private:
    semicolon_statement statement;
    public:
    throw_statement(semicolon_statement statement) : statement(statement) {}
    const semicolon_statement& get_statement();
    static std::pair<throw_statement, std::size_t> parse(const std::vector<lexer::token> &tokens, std::size_t start, symbol_table &symbols);
};
}  // namespace parser
}  // namespace oops_compiler

#endif