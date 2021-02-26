#include "parser.h"

#include "base_classes.h"
#include "class.h"

using namespace oops_compiler::parser;

output<char> parser::parse() {
  output<char> out;
  std::unordered_set<std::string> classes;
  output<source_file> source =
      oops_compiler::parser::parse<source_file>(filename, tokens, 0, classes);
  std::copy(source.messages.begin(), source.messages.end(),
            std::back_inserter(out.messages));
  if (!source.value) {
    return out;
  }
  this->parse_tree = std::move(*source.value);
  output<char> resolved =
      this->parse_tree->resolve_unparsed_method_definitions();
  std::copy(source.messages.begin(), source.messages.end(),
            std::back_inserter(resolved.messages));
  out.value = std::move(resolved.value);
  return out;
}