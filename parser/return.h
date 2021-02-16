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
};
}  // namespace parser
}  // namespace oops_compiler

#endif