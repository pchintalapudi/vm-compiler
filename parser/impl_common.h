#ifndef OOPS_COMPILER_PARSER_IMPL_COMMON_H
#define OOPS_COMPILER_PARSER_IMPL_COMMON_H

#include <sstream>
#include "../logger/logging.h"

#define parse_decl(type)                                      \
  template <>                                                 \
  output<type> oops_compiler::parser::parse<type>(            \
      const char *filename,                                   \
      const std::vector<oops_compiler::lexer::token> &tokens, \
      std::size_t begin)

namespace {

struct message_builder {
  std::stringstream builder;
  oops_compiler::logger::message build_message(
      oops_compiler::logger::level level,
      oops_compiler::logger::context context) {
    auto message = (oops_compiler::logger::message{
        .text = builder.str(),
        .location = context,
        .produced_at = oops_compiler::logger::stage::PARSING,
        .log_level = level});
    builder.clear();
    return message;
  }
};
}  // namespace
#endif