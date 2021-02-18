#ifndef OOPS_COMPILER_PARSER_VARIABLE_H
#define OOPS_COMPILER_PARSER_VARIABLE_H

#include <string>

#include "declaration.h"
#include "modifiers.h"
#include "type.h"

namespace oops_compiler {
namespace parser {

class variable {
 private:
  modifiers mods;
  storage store;
  bool fin;
  declaration dec;

 public:
  variable(modifiers mods, storage store, bool fin, declaration dec)
      : mods(mods), store(store), fin(fin), dec(std::move(dec)) {}
  modifiers get_access_modifier() const { return mods; }
  storage get_storage() const { return store; }
  bool is_final() const { return fin; }
  const declaration &get_declaration() const { return dec; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif