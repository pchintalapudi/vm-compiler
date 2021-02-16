#ifndef OOPS_COMPILER_PARSER_FOR_H
#define OOPS_COMPILER_PARSER_FOR_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class for_statement : public statement {
    private:
    public:
    const statement &get_declaration();
    const expression& get_conditional();
    const expression& get_increment();
    const statement& get_body();
    static std::pair<for_statement, std::size_t> parse(const std::vector<lexer::token> &tokens, std::size_t start, symbol_table &symbols);
};
}  // namespace parser
}  // namespace oops_compiler

#endif