#ifndef OOPS_COMPILER_PARSER_SYMBOL_TABLE_H
#define OOPS_COMPILER_PARSER_SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "type.h"

namespace oops_compiler {
namespace parser {
class symbol_table {
 public:
  typedef std::variant<std::string, type_declaration> type;

 private:
  std::vector<std::unordered_map<std::string, type>> frames;

 public:
  std::optional<type> lookup_symbol(std::string name) {
    for (std::size_t i = frames.size(); i-- > 0;) {
      if (auto it = frames[i].find(name); it != frames[i].end()) {
        return it->second;
      }
    }
    return {};
  }
  void insert_symbol(std::string name, type t) { frames.back()[name] = t; }
  void push_frame() { frames.emplace_back(); }
  void pop_frame() { frames.pop_back(); }
};
}  // namespace parser
}  // namespace oops_compiler

#endif