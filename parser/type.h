#ifndef OOPS_COMPILER_PARSER_TYPE_H
#define OOPS_COMPILER_PARSER_TYPE_H

#include <string>
#include <vector>

namespace oops_compiler {
namespace parser {
class type_instantiation;
class generic_declaration {
 public:
  enum class bound { SUPER, EXTENDS };

 private:
  std::string name;
  std::vector<std::pair<bound, const type_instantiation *>> bounds;

 public:
  generic_declaration(
      std::string name,
      std::vector<std::pair<bound, const type_instantiation *>> bounds)
      : name(std::move(name)), bounds(std::move(bounds)) {}
  const std::string &get_name() const { return name; }
  const std::vector<std::pair<bound, const type_instantiation *>> &get_bounds()
      const {
    return bounds;
  }
};
class type_declaration {
 private:
  std::string name;
  std::vector<generic_declaration> generics;

 public:
  type_declaration(std::string name, std::vector<generic_declaration> generics)
      : name(std::move(name)), generics(std::move(generics)) {}
  const std::string &get_name() const { return name; }
  const std::vector<generic_declaration> &get_generics() const {
    return generics;
  }
  virtual ~type_declaration() = default;
};
class type_instantiation {
 private:
  std::string alias;
  std::vector<type_instantiation> instantiations;
  bool arraytype;

 public:
  type_instantiation(
      std::string alias,
      decltype(type_instantiation::instantiations) instantiations,
      bool arraytype)
      : alias(std::move(alias)),
        instantiations(std::move(instantiations)),
        arraytype(arraytype) {}
  const std::string &get_name() { return alias; }
  const decltype(type_instantiation::instantiations) &
  get_generic_instantiations() const {
    return instantiations;
  }
  bool is_array_type() const { return arraytype; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif