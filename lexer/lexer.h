#ifndef OOPS_COMPILER_LEXER_LEXER_H
#define OOPS_COMPILER_LEXER_LEXER_H

#include <string>
#include <variant>
#include <vector>

#include "../logger/logging.h"
#include "operator_trie.h"
#include "token.h"

namespace oops_compiler {
namespace lexer {

struct lexed_output {
  const char *filename;
  std::vector<logger::message> messages;
  std::vector<token> output;
};
class lexer {
 private:
  operator_trie root;
  const char *file;
  std::size_t file_size;
  const char *filename;

 public:
  lexer(const void *file, std::size_t file_size, const char *filename);

  lexed_output lex();
};
}  // namespace lexer
}  // namespace oops_compiler

#endif