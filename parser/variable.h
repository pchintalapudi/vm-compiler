#ifndef OOPS_COMPILER_PARSER_VARIABLE_H
#define OOPS_COMPILER_PARSER_VARIABLE_H

#include <string>

#include "modifiers.h"
#include "type.h"

namespace oops_compiler {
namespace parser {

class variable {
 public:
  modifiers get_access_modifier() const;
  storage get_storage() const;
  const type_instantiation &get_type() const;
  std::string get_name() const;
};
}  // namespace parser
}  // namespace oops_compiler

#endif