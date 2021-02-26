#ifndef OOPS_COMPILER_PARSER_PARSER_H
#define OOPS_COMPILER_PARSER_PARSER_H

#include "base_classes.h"

namespace oops_compiler {
namespace parser {
class parser {
 private:
  const char *filename;
  std::vector<lexer::token> tokens;
  std::unique_ptr<source_file> parse_tree;

 public:
  parser(const char *filename, std::vector<lexer::token> tokens)
      : filename(filename), tokens(std::move(tokens)), parse_tree() {}
  output<char> parse();
  source_file &get_parse_tree() { return *parse_tree; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif