#include "base.h"
#include "output.h"

using namespace oops_compiler::parser;

template <>
output<std::unique_ptr<identifier>> oops_compiler::parser::parse<identifier>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  (void)classes;
  output<std::unique_ptr<identifier>> out{{}, {}, true};
  if (auto token =
          tokens.pop_type<lexer::token::data::type::IDENTIFIER_TOKEN>()) {
    out.value = std::make_unique<identifier>(
        token->token_data.as_identifier.start,
        token->token_data.as_identifier.size, std::move(token->token_context));
  }
  out.fatal = false;
  return out;
}