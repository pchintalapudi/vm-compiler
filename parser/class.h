#ifndef OOPS_COMPILER_PARSER_CLASS_H
#define OOPS_COMPILER_PARSER_CLASS_H

#include <memory>
#include <optional>
#include <vector>

#include "../logger/logging.h"
#include "access_expression.h"
#include "base_classes.h"
#include "method.h"
#include "type.h"
#include "variable.h"

namespace oops_compiler {
namespace parser {

class classloader;
class class_definition {
 private:
  type_declaration decl;
  std::vector<class_definition> sub_classes;
  std::optional<general_type> super;
  std::vector<general_type> interfaces;
  std::vector<std::unique_ptr<variable>> vars;
  std::vector<std::unique_ptr<method_declaration>> mtds;

 public:
  class_definition(type_declaration decl,
                   std::vector<class_definition> sub_classes,
                   decltype(interfaces) interfaces, decltype(super) super,
                   decltype(vars) vars, decltype(mtds) mtds)
      : decl(std::move(decl)),
        sub_classes(std::move(sub_classes)),
        super(std::move(super)),
        interfaces(std::move(interfaces)),
        vars(std::move(vars)),
        mtds(std::move(mtds)) {}
  const decltype(sub_classes) &inner_classes() const { return sub_classes; }
  const decltype(super) &superclass() const { return super; }
  const decltype(interfaces) &implemented_interfaces() const {
    return interfaces;
  }
  const decltype(vars) &variables() const { return vars; }
  const decltype(mtds) &methods() const { return mtds; }
  virtual ~class_definition() = default;
};
}  // namespace parser
}  // namespace oops_compiler

#endif