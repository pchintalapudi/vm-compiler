#ifndef OOPS_COMPILER_PARSER_METHOD_H
#define OOPS_COMPILER_PARSER_METHOD_H

#include <vector>

#include "basic_block.h"
#include "modifiers.h"
#include "statement.h"
#include "type.h"

namespace oops_compiler {
namespace parser {
class method_declaration : public type_declaration {
 public:
  enum class type { DEF, GET, SET, OPERATOR };

 private:
 public:
  modifiers get_access_modifier() const;
  storage get_storage() const;
  special get_special() const;
  bool is_final() const;
  const type_instantiation &get_return_type() const;
  const std::vector<type_instantiation> &get_parameter_types() const;
};
class unparsed_method_declaration : public method_declaration {
 private:
 public:
  const std::vector<lexer::token> &get_tokens();
};
class method_definition : public method_declaration {
 private:
 public:
  const basic_block &get_method_body() const;
};
}  // namespace parser
}  // namespace oops_compiler

#endif