#ifndef OOPS_COMPILER_PARSER_CLASS_H
#define OOPS_COMPILER_PARSER_CLASS_H

#include <memory>
#include <optional>
#include <vector>

#include "../logger/logging.h"
#include "base_classes.h"
#include "method.h"
#include "scope.h"
#include "type.h"
#include "variable.h"

namespace oops_compiler {
namespace parser {

class classloader;
class class_definition : public type_declaration {
 private:
  const source_file *source;
  std::vector<const class_definition *> sub_classes, interfaces;
  const class_definition *super;
  std::vector<std::unique_ptr<variable>> vars;
  std::vector<std::unique_ptr<method_declaration>> mtds;
  scope class_scope;

 public:
  class_definition(const source_file &source, std::string name,
                   std::vector<generic_declaration> generics,
                   std::vector<const class_definition *> sub_classes,
                   std::vector<const class_definition *> interfaces,
                   const class_definition &super,
                   std::vector<std::unique_ptr<variable>> vars,
                   std::vector<std::unique_ptr<method_declaration>> mtds)
      : type_declaration(std::move(name), std::move(generics)),
        source(&source),
        sub_classes(std::move(sub_classes)),
        interfaces(std::move(interfaces)),
        super(&super),
        vars(std::move(vars)),
        mtds(std::move(mtds)) {}
  const std::vector<const class_definition *> &inner_classes() const {
    return sub_classes;
  }
  std::optional<const class_definition *> superclass() const {
    return super ? std::optional{super} : std::optional<decltype(super)>{};
  }
  const std::vector<const class_definition *> &implemented_interfaces() const {
    return interfaces;
  }
  const std::vector<std::unique_ptr<variable>> &variables() const {
    return vars;
  }
  const std::vector<std::unique_ptr<method_declaration>> &methods() const {
    return mtds;
  }
  const source_file &get_source() const { return *source; }
  std::vector<logger::message> resolve_definitions(classloader &loader);
};
}  // namespace parser
}  // namespace oops_compiler

#endif