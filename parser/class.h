#ifndef OOPS_COMPILER_PARSER_CLASS_H
#define OOPS_COMPILER_PARSER_CLASS_H

#include <memory>
#include <optional>
#include <vector>

#include "base_classes.h"
#include "method.h"
#include "scope.h"
#include "type.h"
#include "variable.h"

namespace oops_compiler {
namespace parser {

class classloader;
class class_definition : public type_declaration,
                         public parseable<class_definition> {
 private:
  std::vector<const class_definition *> sub_classes, interfaces, imported;
  const class_definition *super;
  std::vector<std::string> pack;
  std::vector<std::unique_ptr<variable>> vars;
  std::vector<std::unique_ptr<method_declaration>> mtds;

 public:
  class_definition(std::string name, std::vector<generic_declaration> generics,
                   std::vector<const class_definition *> sub_classes,
                   std::vector<const class_definition *> interfaces,
                   std::vector<const class_definition *> imported,
                   const class_definition &super, std::vector<std::string> pack,
                   std::vector<std::unique_ptr<variable>> vars,
                   std::vector<std::unique_ptr<method_declaration>> mtds)
      : type_declaration(std::move(name), std::move(generics)),
        sub_classes(std::move(sub_classes)),
        interfaces(std::move(interfaces)),
        imported(std::move(imported)),
        super(&super),
        pack(std::move(pack)),
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
  const std::vector<const class_definition *> &imports() const {
    return imported;
  }
  const std::vector<std::string> &package() const { return pack; }
  void resolve_definitions(classloader &loader);
};
}  // namespace parser
}  // namespace oops_compiler

#endif