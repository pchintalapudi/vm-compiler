#ifndef OOPS_COMPILER_PARSER_BASIC_BLOCK_H
#define OOPS_COMPILER_PARSER_BASIC_BLOCK_H

#include <vector>

#include "statement.h"

namespace oops_compiler {
namespace parser {
class basic_block : public statement {
    private:
    public:
    const std::vector<statement> get_substatements() const;
};
}  // namespace parser
}  // namespace oops_compiler

#endif