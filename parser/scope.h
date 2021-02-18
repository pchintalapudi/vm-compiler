#ifndef OOPS_COMPILER_PARSER_SCOPE_H
#define OOPS_COMPILER_PARSER_SCOPE_H

#include <string>
#include <unordered_map>

namespace oops_compiler {
namespace parser {
class scope {
 public:
  enum class type { VARIABLE, METHOD, CLASS, __COUNT__ };

 private:
  scope *parent;
  std::unordered_map<std::string, type> variables;
  class proxy {
   private:
    scope *variables;
    const std::string *key;

   public:
    proxy(scope &s, const std::string &key) : variables(&s), key(&key) {}
    operator type() {
      if (auto it = variables->variables.find(*key);
          it != variables->variables.end()) {
        return it->second;
      } else if (variables->parent) {
        return (*variables->parent)[*key];
      } else {
        return type::__COUNT__;
      }
    }
    proxy &operator=(type t) {
      this->variables->variables[*key] = t;
      return *this;
    }
  };

 public:
  scope() : parent(nullptr) {}
  void set_parent(scope &parent) { this->parent = &parent; }
  proxy operator[](const std::string &str) { return proxy(*this, str); }
  scope extend() { return scope(*this); }
};
}  // namespace parser
}  // namespace oops_compiler

#endif