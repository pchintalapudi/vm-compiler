#ifndef OOPS_COMPILER_PARSER_IF_H
#define OOPS_COMPILER_PARSER_IF_H

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class if_statement : public statement {
    private:
    public:
    const expression& get_conditional();
    const statement& get_statement();
};
}  // namespace parser
}  // namespace oops_compiler

#endif