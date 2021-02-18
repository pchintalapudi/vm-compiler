#ifndef OOPS_COMPILER_PARSER_METHOD_H
#define OOPS_COMPILER_PARSER_METHOD_H

#include <vector>

#include "basic_block.h"
#include "modifiers.h"
#include "scope.h"
#include "type.h"

namespace oops_compiler {
namespace parser {
class class_definition;
class method_declaration : public type_declaration {
 public:
  enum class type { DEF, GET, SET, OPERATOR };
  modifiers mods;
  storage store;
  special spec;
  bool fin;
  const type_instantiation *ret;
  std::vector<const type_instantiation *> parameters;
  const class_definition *host;
  scope argument_scope;

 private:
 public:
  method_declaration(std::string name,
                     std::vector<generic_declaration> generics, modifiers mods,
                     storage store, special spec, bool fin,
                     const type_instantiation &ret,
                     std::vector<const type_instantiation *> parameters,
                     const class_definition &host, scope method_scope)
      : type_declaration(std::move(name), std::move(generics)),
        mods(mods),
        store(store),
        spec(spec),
        fin(fin),
        ret(&ret),
        parameters(std::move(parameters)),
        host(&host),
        argument_scope(method_scope) {}
  modifiers get_access_modifier() const { return mods; }
  storage get_storage() const { return store; }
  special get_special() const { return spec; }
  scope &get_scope() {
    return argument_scope;
  }
  bool is_final() const { return fin; }
  const type_instantiation &get_return_type() const { return *ret; }
  const std::vector<const type_instantiation *> &get_parameter_types() const {
    return parameters;
  }
  const class_definition &get_class() const { return *host; }
};
class unparsed_method_declaration : public method_declaration {
 private:
  std::vector<lexer::token> tokens;

 public:
  unparsed_method_declaration(
      std::string name, std::vector<generic_declaration> generics,
      modifiers mods, storage store, special spec, bool fin,
      const type_instantiation &ret,
      std::vector<const type_instantiation *> parameters,
      const class_definition &host, scope method_scope,
      std::vector<lexer::token> tokens)
      : method_declaration(std::move(name), std::move(generics), mods, store,
                           spec, fin, ret, parameters, host,
                           std::move(method_scope)),
        tokens(std::move(tokens)) {}
  const std::vector<lexer::token> &get_tokens() { return tokens; }
};
class method_definition : public method_declaration {
 private:
  basic_block body;

 public:
  method_definition(unparsed_method_declaration &&unparsed, basic_block parsed)
      : method_declaration(std::move(unparsed)), body(std::move(parsed)) {}
  const basic_block &get_method_body() const { return body; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif