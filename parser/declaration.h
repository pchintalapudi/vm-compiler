#ifndef OOPS_COMPILER_PARSER_DECLARATION_H
#define OOPS_COMPILER_PARSER_DECLARATION_H

#include <string>

#include "expression.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class declaration : public statement {
    private:
    public:
    const std::string &variable_name();
    const expression& get_expression();
};
}  // namespace parser
}  // namespace oops_compiler

#endif