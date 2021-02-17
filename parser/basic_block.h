#ifndef OOPS_COMPILER_PARSER_BASIC_BLOCK_H
#define OOPS_COMPILER_PARSER_BASIC_BLOCK_H

#include <vector>

#include "statement.h"

namespace oops_compiler {
namespace parser {
class basic_block : public statement {
 private:
  std::vector<statement> substatements;

 public:
  basic_block(std::vector<statement> substatements)
      : substatements(substatements) {}
  const std::vector<statement> &get_substatements() const;
};
}  // namespace parser
}  // namespace oops_compiler

#endif