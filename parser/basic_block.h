#ifndef OOPS_COMPILER_PARSER_BASIC_BLOCK_H
#define OOPS_COMPILER_PARSER_BASIC_BLOCK_H

#include <memory>
#include <utility>
#include <vector>

#include "base_classes.h"
#include "statement.h"

namespace oops_compiler {
namespace parser {
class basic_block : public statement, public parseable<basic_block> {
 private:
  scope locals;
  std::vector<std::unique_ptr<statement>> substatements;

 public:
  basic_block(scope locals,
              std::vector<std::unique_ptr<statement>> substatements)
      : locals(std::move(locals)), substatements(std::move(substatements)) {}
  const scope &get_locals() { return locals; }
  const std::vector<std::unique_ptr<statement>> &get_substatements() const {
    return this->substatements;
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif