#ifndef OOPS_COMPILER_PARSER_ENHANCED_FOR_H
#define OOPS_COMPILER_PARSER_ENHANCED_FOR_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class enhanced_for_statement : public statement {
    private:
    public:
    const statement &get_declaration();
    const expression& get_iterable();
    const statement& get_body();
    static std::pair<enhanced_for_statement, std::size_t> parse(std::vector<lexer::token> tokens, std::size_t start);
};
}  // namespace parser
}  // namespace oops_compiler

#endif