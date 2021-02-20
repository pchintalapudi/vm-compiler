#ifndef OOPS_COMPILER_PARSER_CLASSLOADER_H
#define OOPS_COMPILER_PARSER_CLASSLOADER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "class.h"

namespace oops_compiler {
namespace parser {
class classloader {
 private:
  std::unordered_map<std::string, std::unique_ptr<class_definition>> defined,
      declared;
  std::unique_ptr<class_definition> load_class_declaration(
      const std::string &name);

 public:
  explicit operator bool() const { return declared.size(); }
  class_definition *lookup_class(const std::vector<std::string> &path) {
    std::string name;
    for (const auto &str : path) {
      name += str;
      name += '.';
    }
    name.pop_back();
    if (auto it = defined.find(name); it != defined.end()) {
      return it->second.get();
    } else if (auto it2 = declared.find(name); it2 != declared.end()) {
      return it2->second.get();
    } else {
      return (declared[name] = load_class_declaration(name)).get();
    }
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif
