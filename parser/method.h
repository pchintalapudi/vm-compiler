#ifndef OOPS_COMPILER_PARSER_METHOD_H
#define OOPS_COMPILER_PARSER_METHOD_H

#include <vector>

#include "basic_block.h"
#include "expression.h"
#include "modifiers.h"
#include "type.h"

namespace oops_compiler {
namespace parser {
class class_definition;
class argument {
 private:
  type_instantiation type;
  std::string name;
  std::unique_ptr<expression> default_value;

 public:
  argument(type_instantiation type, std::string name,
           std::optional<std::unique_ptr<expression>> default_value)
      : type(std::move(type)),
        name(std::move(name)),
        default_value(default_value ? std::move(*default_value)
                                    : std::unique_ptr<expression>{}) {}

  const type_instantiation &get_type() const { return type; }
  const std::string &get_name() const { return name; }
};
class method_declaration {
 public:
  enum class type { DEF, GET, SET, OPERATOR, CONSTRUCTOR };

 private:
  type_declaration decl;
  modifiers mods;
  storage store;
  special spec;
  bool fin;
  type_instantiation ret;
  std::vector<argument> parameters;

 public:
  method_declaration(type_declaration decl, modifiers mods, storage store,
                     special spec, bool fin, type_instantiation ret,
                     decltype(method_declaration::parameters) parameters)
      : decl(std::move(decl)),
        mods(mods),
        store(store),
        spec(spec),
        fin(fin),
        ret(std::move(ret)),
        parameters(std::move(parameters)) {}
  const std::string &get_name() const { return decl.get_name(); }
  modifiers get_access_modifier() const { return mods; }
  storage get_storage() const { return store; }
  special get_special() const { return spec; }
  bool is_final() const { return fin; }
  const type_instantiation &get_return_type() const { return ret; }
  const decltype(parameters) &get_parameters() const { return parameters; }
};
class unparsed_method_declaration : public method_declaration {
 private:
  std::vector<lexer::token> tokens;

 public:
  unparsed_method_declaration(type_declaration decl, modifiers mods,
                              storage store, special spec, bool fin,
                              type_instantiation ret,
                              std::vector<argument> parameters,
                              std::vector<lexer::token> tokens)
      : method_declaration(std::move(decl), mods, store, spec, fin,
                           std::move(ret), std::move(parameters)),
        tokens(std::move(tokens)) {}
  std::vector<lexer::token> &get_tokens() { return tokens; }
};
class method_definition : public method_declaration {
 private:
  std::optional<basic_block> body;

 public:
  method_definition(unparsed_method_declaration &&unparsed, std::optional<basic_block> parsed)
      : method_declaration(std::move(unparsed)), body(std::move(parsed)) {}
  const std::optional<basic_block> &get_method_body() const { return body; }
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