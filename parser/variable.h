#ifndef OOPS_COMPILER_PARSER_VARIABLE_H
#define OOPS_COMPILER_PARSER_VARIABLE_H

#include <string>

#include "modifiers.h"
#include "type.h"

namespace oops_compiler {
namespace parser {

class variable {
 private:
  modifiers mods;
  storage store;
  const type_instantiation *type;
  std::string name;

 public:
  modifiers get_access_modifier() const { return mods; }
  storage get_storage() const { return store; }
  const type_instantiation &get_type() const { return *type; }
  const std::string &get_name() const { return name; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif