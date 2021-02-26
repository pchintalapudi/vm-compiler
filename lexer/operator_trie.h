#ifndef OOPS_COMPILER_LEXER_OPERATOR_TRIE_H
#define OOPS_COMPILER_LEXER_OPERATOR_TRIE_H

#include <array>
#include <optional>
#include <vector>

#include "converters.h"

namespace oops_compiler {
namespace lexer {
class operator_trie {
 private:
  operator_trie *parent;
  std::array<operator_trie *, 128> subtries;
  int depth;
  operators full_operator;
  bool parent_full_operator;
  operator_trie(int depth, operator_trie *parent)
      : parent(parent),
        subtries(),
        depth(depth),
        full_operator(operators::__COUNT__),
        parent_full_operator(false) {}
  void mark_parent_full();

 public:
  operator_trie() : operator_trie(0, nullptr) {}
  operator_trie(const operator_trie &other) = delete;
  operator_trie &operator=(const operator_trie &other) = delete;
  operator_trie(operator_trie &&sacrifice);
  operator_trie &operator=(operator_trie &&sacrifice) {
    std::swap(*this, sacrifice);
    return *this;
  }
  void insert(const char *str, operators op);
  operator_trie *&operator[](char c) {
    return subtries[std::min(static_cast<int>(c), 0x7f)];
  }
  const operator_trie *operator[](char c) const {
    return subtries[std::min(static_cast<int>(c), 0x7f)];
  }
  int get_depth() const { return depth; }

  operator_trie *get_parent() { return parent; }
  const operator_trie *get_parent() const { return parent; }
  operators get_operator() const { return this->full_operator; }
  bool parent_full() const { return this->parent_full_operator; }

  void get_elements(std::vector<std::string> &out, std::string &root) const {
    if (this->full_operator != operators::__COUNT__) {
      out.push_back(root);
    }
    root.push_back('\0');
    for (std::size_t i = 0; i < this->subtries.size(); i++) {
      if (this->subtries[i]) {
        root[root.size() - 1] = static_cast<char>(i);
        this->subtries[i]->get_elements(out, root);
      }
    }
    root.pop_back();
  }

  ~operator_trie();
};
}  // namespace lexer
}  // namespace oops_compiler

#endif