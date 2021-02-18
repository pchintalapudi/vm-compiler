#include <sstream>

#include "basic_block.h"
#include "classloader.h"
#include "expression.h"
#include "ifelse.h"
#include "impl_common.h"
#include "returns.h"
#include "scope.h"
#include "semicolon.h"
#include "statement.h"
#include "switch.h"
#include "throw.h"
#include "whiles.h"

using namespace oops_compiler::parser;

parse_decl(statement);
parse_decl(basic_block);
parse_decl(while_statement);
parse_decl(if_statement);
parse_decl(else_statement);
parse_decl(return_statement);
parse_decl(throw_statement);
parse_decl(semicolon_statement);

parse_decl(while_statement) {
  output<while_statement> out;
  out.filename = filename;
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
  out.filename = filename;
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
  out.filename = filename;
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

parse_decl(basic_block) {
  output<basic_block> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  begin++;
  if (tokens.size() == begin) {
    message_builder builder;
    builder.builder << "Missing closing brace in statement!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token - 1].token_context));
    return out;
  }
  auto subscope = scope.extend();
  if (tokens[begin].token_data.token_type ==
          lexer::token::data::type::OPERATOR_TOKEN &&
      tokens[begin].token_data.as_operator == lexer::operators::CURLY_CLOSE) {
    out.contexts.push_back(tokens[begin].token_context);
    out.next_token = begin + 1;
    out.value = std::make_unique<basic_block>(
        subscope, std::vector<std::unique_ptr<statement>>{});
    return out;
  }
  std::vector<std::unique_ptr<statement>> statements;
  do {
    auto stmnt = parse<statement>(filename, tokens, begin, loader, subscope);
    std::copy(stmnt.messages.begin(), stmnt.messages.end(),
              std::back_inserter(out.messages));
    out.next_token = stmnt.next_token;
    if (!stmnt.value) {
      return out;
    }
    statements.push_back(std::move(*stmnt.value));
    begin = out.next_token;
  } while (
      begin < tokens.size() &&
      (tokens[begin].token_data.token_type !=
           lexer::token::data::type::OPERATOR_TOKEN ||
       tokens[begin].token_data.as_operator != lexer::operators::CURLY_CLOSE));
  if (tokens.size() == begin) {
    message_builder builder;
    builder.builder << "Missing closing brace in statement!";
    out.messages.push_back(builder.build_message(
        logger::level::ERROR, tokens[out.next_token - 1].token_context));
  } else {
    out.contexts.push_back(tokens[begin].token_context);
    out.next_token = begin + 1;
  }
  out.value =
      std::make_unique<basic_block>(std::move(subscope), std::move(statements));
  return out;
}

parse_decl(return_statement) {
  output<return_statement> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  begin++;
  auto stmnt =
      parse<semicolon_statement>(filename, tokens, begin, loader, scope);
  std::copy(stmnt.messages.begin(), stmnt.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = stmnt.next_token;
  if (!stmnt.value) {
    return out;
  }
  out.value = std::make_unique<return_statement>(std::move(**stmnt.value));
  return out;
}

parse_decl(throw_statement) {
  output<throw_statement> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  begin++;
  auto stmnt =
      parse<semicolon_statement>(filename, tokens, begin, loader, scope);
  std::copy(stmnt.messages.begin(), stmnt.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = stmnt.next_token;
  if (!stmnt.value) {
    return out;
  }
  out.value = std::make_unique<throw_statement>(std::move(**stmnt.value));
  return out;
}

parse_decl(semicolon_statement) {
  output<semicolon_statement> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  begin++;
  auto expr =
      parse<expression>(filename, tokens, begin, loader, scope);
  std::copy(expr.messages.begin(), expr.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = expr.next_token;
  if (!expr.value) {
    return out;
  }
  out.value = std::make_unique<semicolon_statement>(std::move(*expr.value));
  return out;
}