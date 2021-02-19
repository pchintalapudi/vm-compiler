#ifndef OOPS_COMPILER_PARSER_ACCESS_EXPRESSION_H
#define OOPS_COMPILER_PARSER_ACCESS_EXPRESSION_H

#include "expression.h"
#include "method.h"
#include "type.h"

namespace oops_compiler {
namespace parser {
class access_expression : public expression {
 private:
  std::optional<const type_declaration *> first_class;
  std::vector<std::string> intermediate_accesses;
  std::variant<std::string, method_instantiation> end_access;

 public:
  access_expression(std::optional<const type_declaration *> first_class,
                    std::vector<std::string> intermediate_acesses,
                    std::variant<std::string, method_instantiation> end_access)
      : first_class(std::move(first_class)),
        intermediate_accesses(std::move(intermediate_accesses)),
        end_access(std::move(end_access)) {}

  std::optional<const type_declaration *> get_first_class() const {
    return first_class;
  }

  const std::vector<std::string> &get_intermediate_accesses() const {
    return intermediate_accesses;
  }

  const std::variant<std::string, method_instantiation> &get_end_access()
      const {
    return end_access;
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif