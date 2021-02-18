#ifndef OOPS_COMPILER_PARSER_SCOPE_H
#define OOPS_COMPILER_PARSER_SCOPE_H

#include <string>
#include <unordered_map>

namespace oops_compiler {
namespace parser {
class scope {
 public:
  enum class type { VARIABLE, GENERIC, __COUNT__ };

 private:
  scope *parent;
  std::unordered_map<std::string, std::string> variables;
  std::unordered_map<std::string, std::vector<std::string>> class_aliases;
  static std::vector<std::string> empty;

 public:
  scope() : parent(nullptr) {}
  void set_parent(scope &parent) { this->parent = &parent; }
  scope extend() { return scope(*this); }
  void add_class(std::vector<std::string> path, std::string alias) {
    class_aliases[std::move(alias)] = std::move(path);
  }
  const std::vector<std::string> &get_class(const std::string &alias) {
    if (auto it = class_aliases.find(alias); it != class_aliases.end()) {
      return it->second;
    } else {
      return empty;
    }
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif