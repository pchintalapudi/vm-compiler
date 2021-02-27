#ifndef OOPS_COMPILER_PARSER_TOKEN_STREAM_H
#define OOPS_COMPILER_PARSER_TOKEN_STREAM_H

#include <deque>

#include "../lexer/lexer.h"

namespace oops_compiler {
namespace parser {
class token_stream {
 private:
  std::deque<lexer::token> tokens;

 public:
  std::size_t size() const { return tokens.size(); }

  logger::context context() const {
    return this->size() ? tokens.front().token_context
                        : logger::context::NONE();
  }

  std::optional<lexer::token> pop() {
    if (tokens.size()) {
      lexer::token token = std::move(tokens.front());
      tokens.pop_front();
      return token;
    }
    return {};
  }

  void push(lexer::token token) { tokens.push_front(std::move(token)); }

  template <lexer::token::data::type... ts>
  std::optional<lexer::token> pop_type() {
    auto token = this->pop();
    if (token && ((token->token_data.token_type != ts) && ...)) {
      this->push(std::move(*token));
      return {};
    }
    return token;
  }

  template <lexer::token::data::type tp, typename... ts>
  std::enable_if_t<(sizeof...(ts) > 0), std::optional<lexer::token>> pop_specific(ts... args) {
    auto token = this->pop_type<tp>();
    if (token && ((token->token_data.template as<ts>() != args) && ...)) {
      this->push(std::move(*token));
      return {};
    }
    return token;
  }

  template <lexer::token::data::type tp, typename... ts>
  std::optional<lexer::token> pop_until(ts... args) {
    std::optional<lexer::token> token;
    do {
      token = this->pop();
    } while (token && (token->token_data.token_type != tp ||
                       ((token->token_data.template as<ts>() != args) && ...)));
    return token;
  }

  void enqueue(lexer::token token) {
    this->tokens.push_back(std::move(token));
  }
};
}  // namespace parser
}  // namespace oops_compiler

#endif