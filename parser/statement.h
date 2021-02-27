#ifndef OOPS_COMPILER_PARSER_STATEMENT_H
#define OOPS_COMPILER_PARSER_STATEMENT_H

#include "base.h"

namespace oops_compiler {
namespace parser {
struct statement : virtual ast_node {};

struct basic_block : virtual statement,
                     virtual visitable<basic_block, statement> {
  using visitable<basic_block, statement>::visit;
};
}  // namespace parser
}  // namespace oops_compiler

#endif