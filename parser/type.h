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
 public:
  const std::string &get_name() const;
  const std::vector<std::pair<bound, const type_instantiation *>> &get_bounds()
      const;
};
class type_declaration {
 private:
 public:
  const std::string &get_name() const;
  const std::vector<generic_declaration> &get_generics() const;
  virtual ~type_declaration();
};
class type_instantiation : public type_declaration {
 private:
 public:
  const std::vector<const type_instantiation *> &get_generic_instantiations()
      const;
};
}  // namespace parser
}  // namespace oops_compiler

#endif