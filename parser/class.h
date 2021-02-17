#ifndef OOPS_COMPILER_PARSER_CLASS_H
#define OOPS_COMPILER_PARSER_CLASS_H

#include <vector>

#include "method.h"
#include "type.h"
#include "variable.h"

namespace oops_compiler {
namespace parser {
class class_definition : public type_declaration {
 private:
 public:
  const std::vector<class_definition> &inner_classes();
  const class_definition *superclass();
  const std::vector<class_definition> &implemented_interfaces();
  const std::vector<variable> &variables();
  const std::vector<method_declaration> &methods();
  const std::vector<class_definition> &imports();
  const std::vector<std::string> &package();
};
}  // namespace parser
}  // namespace oops_compiler

#endif