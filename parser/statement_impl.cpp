#include <sstream>

#include "basic_block.h"
#include "classloader.h"
#include "expression.h"
#include "ifelse.h"
#include "scope.h"
#include "statement.h"
#include "whiles.h"

using namespace oops_compiler::parser;

#define parse_decl(type)                                      \
  template <>                                                 \
  output<type> oops_compiler::parser::parse<type>(            \
      const char *filename,                                   \
      const std::vector<oops_compiler::lexer::token> &tokens, \
      std::size_t begin, classloader &loader, scope &scope)

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

parse_decl(while_statement) {
  output<while_statement> out;
  out.contexts.push_back(tokens[begin].token_context);
  if (tokens.size() == ++begin) {
    message_builder builder;
    builder.builder << "Missing conditional and statement in while loop!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[begin - 1].token_context));
    out.next_token = begin;
    return out;
  }
  if (tokens[begin].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[begin].token_data.as_operator != lexer::operators::ROUND_OPEN) {
    message_builder builder;
    builder.builder << "While loop must have a parenthesized condition!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    out.next_token = begin;
    return out;
  }
  auto conditional =
      parse<parenthetical>(filename, tokens, begin, loader, scope);
  std::copy(conditional.messages.begin(), conditional.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = conditional.next_token;
  if (!conditional.value) {
    return out;
  }
  begin = out.next_token;
  auto stmnt = parse<statement>(filename, tokens, begin, loader, scope);
  std::copy(stmnt.messages.begin(), stmnt.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = stmnt.next_token;
  if (!stmnt.value) {
    return out;
  }
  begin = out.next_token;
  out.value = std::make_unique<while_statement>(std::move(*conditional.value),
                                                std::move(*stmnt.value));
}
parse_decl(if_statement) {
  output<if_statement> out;
  out.contexts.push_back(tokens[begin].token_context);
  if (tokens.size() == ++begin) {
    message_builder builder;
    builder.builder << "Missing conditional and statement in if statement!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[begin - 1].token_context));
    out.next_token = begin;
    return out;
  }
  if (tokens[begin].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[begin].token_data.as_operator != lexer::operators::ROUND_OPEN) {
    message_builder builder;
    builder.builder << "If statement must have a parenthesized condition!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    out.next_token = begin;
    return out;
  }
  auto conditional =
      parse<parenthetical>(filename, tokens, begin, loader, scope);
  std::copy(conditional.messages.begin(), conditional.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = conditional.next_token;
  if (!conditional.value) {
    return out;
  }
  begin = out.next_token;
  auto stmnt = parse<statement>(filename, tokens, begin, loader, scope);
  std::copy(stmnt.messages.begin(), stmnt.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = stmnt.next_token;
  if (!stmnt.value) {
    return out;
  }
  begin = out.next_token;
  out.value = std::make_unique<if_statement>(std::move(*conditional.value),
                                             std::move(*stmnt.value));
}
parse_decl(else_statement) {
  output<else_statement> out;
  out.contexts.push_back(tokens[begin].token_context);
  if (tokens.size() == ++begin) {
    message_builder builder;
    builder.builder << "Missing statement in else statement!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[begin - 1].token_context));
    out.next_token = begin;
    return out;
  }
  auto stmnt = parse<statement>(filename, tokens, begin, loader, scope);
  std::copy(stmnt.messages.begin(), stmnt.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = stmnt.next_token;
  if (!stmnt.value) {
    return out;
  }
  begin = out.next_token;
  out.value = std::make_unique<else_statement>(std::move(*stmnt.value));
}
parse_decl(parenthetical) {
  output<parenthetical> out;
  out.contexts.push_back(tokens[begin].token_context);
  auto expr = parse<expression>(filename, tokens, begin + 1, loader, scope);
  std::copy(expr.messages.begin(), expr.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = expr.next_token;
  if (!expr.value) {
    return out;
  }
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Missing closing parenthesis in expession!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token - 1].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[out.next_token].token_data.as_operator !=
          lexer::operators::ROUND_CLOSE) {
    message_builder builder;
    builder.builder << "Missing closing parenthesis in expression!";
    out.messages.push_back(builder.build_message(
        logger::level::ERROR, tokens[out.next_token].token_context));
  }
  out.value = std::make_unique<parenthetical>(*expr.value);
  return out;
}
parse_decl(indexed) {
  output<indexed> out;
  out.contexts.push_back(tokens[begin].token_context);
  auto expr = parse<expression>(filename, tokens, begin + 1, loader, scope);
  std::copy(expr.messages.begin(), expr.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = expr.next_token;
  if (!expr.value) {
    return out;
  }
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Missing closing bracket in expession!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token - 1].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[out.next_token].token_data.as_operator !=
          lexer::operators::SQUARE_CLOSE) {
    message_builder builder;
    builder.builder << "Missing closing bracket in expression!";
    out.messages.push_back(builder.build_message(
        logger::level::ERROR, tokens[out.next_token].token_context));
  }
  out.value = std::make_unique<indexed>(*expr.value);
  return out;
}