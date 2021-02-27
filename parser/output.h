#ifndef OOPS_COMPILER_PARSER_OUTPUT_H
#define OOPS_COMPILER_PARSER_OUTPUT_H

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "../logger/logging.h"
#include "token_stream.h"

namespace oops_compiler {
namespace parser {
template <typename out_t>
struct output {
  std::vector<logger::message> messages;
  std::optional<out_t> value;
  bool fatal;
  template <typename... Args>
  void log(logger::level lvl, logger::context context, Args... args) {
    std::stringstream out;
    (void)((out << ... << args));
    messages.push_back(logger::message{.text = out.str(),
                                       .location = context,
                                       .produced_at = logger::stage::PARSING,
                                       .log_level = lvl});
  }
  template <typename t>
  void merge_messages(output<t> &other) {
    std::move(other.messages.begin(), other.messages.end(),
              std::back_inserter(this->messages));
  }
};

template <typename out_t>
std::conditional_t<std::is_base_of_v<ast_node, out_t>,
                   output<std::unique_ptr<out_t>>, output<out_t>>
parse(token_stream &tokens, std::unordered_set<std::string> &classes);

}  // namespace parser
}  // namespace oops_compiler

#endif