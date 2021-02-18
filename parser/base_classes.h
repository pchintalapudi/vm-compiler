#ifndef OOPS_COMPILER_PARSER_BASE_CLASSES_H
#define OOPS_COMPILER_PARSER_BASE_CLASSES_H

#include <string>
#include <variant>
#include <vector>

#include "../lexer/token.h"
#include "../logger/logging.h"

namespace oops_compiler {
namespace parser {
class classloader;
class scope;
template <typename out_t>
struct output {
  const char *filename;
  std::optional<std::unique_ptr<out_t>> value;
  std::vector<logger::message> messages;
  std::vector<logger::context> contexts;
  std::size_t next_token;
};
template <typename node>
output<node> parse(const char *filename,
                   const std::vector<lexer::token> &tokens, std::size_t begin,
                   classloader &loader, scope &scope);
}  // namespace parser
}  // namespace oops_compiler

#endif