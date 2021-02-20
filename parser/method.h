#ifndef OOPS_COMPILER_PARSER_METHOD_H
#define OOPS_COMPILER_PARSER_METHOD_H

#include <vector>

#include "basic_block.h"
#include "modifiers.h"
#include "type.h"
#include "expression.h"

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

 private:
 public:
  method_declaration(std::string name,
                     std::vector<generic_declaration> generics, modifiers mods,
                     storage store, special spec, bool fin,
                     const type_instantiation &ret,
                     std::vector<const type_instantiation *> parameters,
                     const class_definition &host)
      : type_declaration(std::move(name), std::move(generics)),
        mods(mods),
        store(store),
        spec(spec),
        fin(fin),
        ret(&ret),
        parameters(std::move(parameters)),
        host(&host) {}
  modifiers get_access_modifier() const { return mods; }
  storage get_storage() const { return store; }
  special get_special() const { return spec; }
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
      const class_definition &host,
      std::vector<lexer::token> tokens)
      : method_declaration(std::move(name), std::move(generics), mods, store,
                           spec, fin, ret, parameters, host),
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

class method_instantiation : public expression {
 private:
  std::string name;
  std::vector<const type_instantiation *> generic_instantiations;
  std::vector<std::unique_ptr<expression>> arguments;

 public:
  const std::string &get_name() const { return name; }
  const std::vector<const type_instantiation *> &get_generic_instantiations()
      const {
    return generic_instantiations;
  }
  const std::vector<std::unique_ptr<expression>> &get_arguments() const {
    return arguments;
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif