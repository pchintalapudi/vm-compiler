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

 public:
  scope() : parent(nullptr) {}
  void set_parent(scope &parent) { this->parent = &parent; }
  scope extend() { return scope(*this); }
  std::optional<const std::string *> get_variable(const std::string &arg) {
    if (const auto &it = variables.find(arg); it != variables.end()) {
      return &it->second;
    } else {
      return {};
    }
  }
  void add_class(std::vector<std::string> path, std::string alias) {
    class_aliases[std::move(alias)] = std::move(path);
  }
  std::optional<const std::vector<std::string> *> get_class(
      const std::string &alias) {
    if (const auto &it = class_aliases.find(alias); it != class_aliases.end()) {
      return &it->second;
    } else {
      return {};
    }
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif