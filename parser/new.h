#ifndef OOPS_COMPILER_PARSER_NEW_H
#define OOPS_COMPILER_PARSER_NEW_H

#include "expression.h"
#include "type.h"

namespace oops_compiler {
namespace parser {
struct new_expression : public expression {
  std::unique_ptr<general_type> type;
  std::vector<std::unique_ptr<expression>> arguments;
  bool arraytype;
  new_expression(decltype(type) type, decltype(arguments) arguments,
                 decltype(arraytype) arraytype)
      : type(std::move(type)),
        arguments(std::move(arguments)),
        arraytype(arraytype) {}
};
}  // namespace parser
}  // namespace oops_compiler

#endif