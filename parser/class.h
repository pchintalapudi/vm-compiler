#ifndef OOPS_COMPILER_PARSER_CLASS_H
#define OOPS_COMPILER_PARSER_CLASS_H

#include <memory>
#include <variant>
#include <vector>

#include "base.h"
#include "expression.h"
#include "statement.h"
#include "token_stream.h"
#include "type.h"

namespace oops_compiler {
namespace parser {
struct general_type;
struct expression;
struct type_declaration;
struct basic_block;
enum class access { PUBLIC, PROTECTED, PACKAGE, PRIVATE };
struct class_variable : virtual ast_node,
                        virtual visitable<class_variable, ast_node> {
  access modifier;
  bool is_static;
  bool is_final;
  std::unique_ptr<general_type> type;
  std::unique_ptr<identifier> name;
  std::unique_ptr<expression> initializer;
  logger::context context;
  class_variable(decltype(modifier) modifier, decltype(is_static) is_static,
                 decltype(is_final) is_final, decltype(type) type,
                 decltype(name) name, decltype(initializer) initializer,
                 decltype(context) context)
      : modifier(modifier),
        is_static(is_static),
        is_final(is_final),
        type(std::move(type)),
        name(std::move(name)),
        initializer(std::move(initializer)),
        context(std::move(context)) {}
  using visitable<class_variable, ast_node>::visit;
};
struct parameter : virtual ast_node, virtual visitable<parameter, ast_node> {
  std::unique_ptr<general_type> type;
  std::unique_ptr<identifier> name;
  logger::context context;
  parameter(decltype(type) type, decltype(name) name, decltype(context) context)
      : type(std::move(type)),
        name(std::move(name)),
        context(std::move(context)) {}
  using visitable<parameter, ast_node>::visit;
};
struct class_method : virtual ast_node,
                      virtual visitable<class_method, ast_node> {
  std::unique_ptr<general_type> return_type;
  std::unique_ptr<type_declaration> declaration;
  std::vector<std::unique_ptr<parameter>> parameters;
  std::variant<token_stream, std::unique_ptr<basic_block>> body;
  access modifier;
  bool is_static;
  bool is_final;
  bool is_native;
  bool is_intrinsic;
  bool is_abstract;
  logger::context context;
  class_method(decltype(return_type) return_type,
               decltype(declaration) declaration,
               decltype(parameters) parameters, decltype(body) body,
               decltype(modifier) modifier, decltype(is_static) is_static,
               decltype(is_native) is_native,
               decltype(is_intrinsic) is_intrinsic,
               decltype(is_abstract) is_abstract, decltype(context) context)
      : return_type(std::move(return_type)),
        declaration(std::move(declaration)),
        parameters(std::move(parameters)),
        body(std::move(body)),
        modifier(std::move(modifier)),
        is_static(std::move(is_static)),
        is_native(std::move(is_native)),
        is_intrinsic(std::move(is_intrinsic)),
        is_abstract(std::move(is_abstract)),
        context(std::move(context)) {}
  using visitable<class_method, ast_node>::visit;
};
struct class_definition : virtual ast_node,
                          virtual visitable<class_definition, ast_node> {
  std::unique_ptr<type_declaration> declaration;
  std::vector<std::unique_ptr<class_variable>> variables;
  std::vector<std::unique_ptr<class_method>> methods;
  std::vector<std::unique_ptr<class_definition>> inner_classes;
  logger::context context;
  class_definition(decltype(declaration) declaration,
                   decltype(variables) variables, decltype(methods) methods,
                   decltype(inner_classes), decltype(context) context)
      : declaration(std::move(declaration)),
        variables(std::move(variables)),
        methods(std::move(methods)),
        inner_classes(std::move(inner_classes)),
        context(std::move(context)) {}
  using visitable<class_definition, ast_node>::visit;
};
}  // namespace parser
}  // namespace oops_compiler

#endif