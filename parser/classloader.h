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
  std::vector<class_definition *> resolve_unloaded_classes() {
    while (*this) {
      auto it = this->declared.begin();
      auto name = it->first;
      it->second->resolve_definitions(*this);
      it = this->declared.find(name);
      this->defined[name] = std::move(it->second);
      this->declared.erase(it);
    }
    std::vector<class_definition *> out;
    out.reserve(this->defined.size());
    for (auto &pair : defined) {
      out.push_back(pair.second.get());
    }
    return out;
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif
