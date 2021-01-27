#include "operator_trie.h"

#include <utility>

using namespace oops_compiler::lexer;

void operator_trie::insert(const char *str, operators op) {
  if (*str == '\0') {
    this->full_operator = op;
    for (unsigned i = 0; i < this->subtries.size(); i++) {
      if (this->subtries[i]) {
        this->subtries[i]->mark_parent_full();
      }
    }
  } else {
    if (!(*this)[*str]) {
      (*this)[*str] = new operator_trie(this->depth + 1, this);
    }
    (*this)[*str]->insert(str + 1, op);
  }
}

void operator_trie::mark_parent_full() {
  this->parent_full_operator = true;
  for (unsigned i = 0; i < this->subtries.size(); i++) {
    if (this->subtries[i]) {
      this->subtries[i]->mark_parent_full();
    }
  }
}

operator_trie::operator_trie(operator_trie &&sacrifice) {
  this->depth = sacrifice.depth;
  this->full_operator = sacrifice.full_operator;
  this->subtries = sacrifice.subtries;
  std::fill(sacrifice.subtries.begin(), sacrifice.subtries.end(), nullptr);
}
operator_trie::~operator_trie() {
  for (auto ptr : this->subtries) {
    delete ptr;
  }
}