#include <unordered_set>

#include "expression.h"
#include "impl_common.h"
#include "loop_control.h"
#include "nary_expression.h"

using namespace oops_compiler::parser;

parse_decl(expression);
parse_decl(parenthetical);

parse_decl(parenthetical) {
  output<parenthetical> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  auto expr = parse<expression>(filename, tokens, begin + 1);
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
  out.value = std::make_unique<parenthetical>(std::move(*expr.value));
  out.contexts.push_back(tokens[out.next_token].token_context);
  out.next_token++;
  return out;
}

parse_decl(break_expression) {
  output<break_expression> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  out.next_token = begin + 1;
  out.value = std::make_unique<break_expression>();
  return out;
}

parse_decl(continue_expression) {
  output<continue_expression> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  out.next_token = begin + 1;
  out.value = std::make_unique<continue_expression>();
  return out;
}

parse_decl(identifier_expression) {
  output<identifier_expression> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  out.next_token = begin + 1;
  out.value = std::make_unique<identifier_expression>(
      tokens[begin].token_data.as_deferred.start,
      tokens[begin].token_data.as_deferred.size);
  return out;
}

parse_decl(literal_expression) {
  output<literal_expression> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  out.next_token = begin + 1;
  out.value = std::make_unique<literal_expression>(tokens[begin]);
  return out;
}

namespace {
struct expression_tree {
  expression_tree *parent;
  oops_compiler::lexer::token token;
  std::unique_ptr<expression_tree> left, right;
};

}  // namespace

parse_decl(expression) {
  switch (tokens[begin].token_data.token_type) {
    case lexer::token::data::type::KEYWORD_TOKEN:
      switch (tokens[begin].token_data.as_keyword) {
        case lexer::keywords::BREAK:
          return output<expression>::generalize(
              parse<break_expression>(filename, tokens, begin));
        case lexer::keywords::CONTINUE:
          return output<expression>::generalize(parse<continue_expression>(
              filename, tokens, begin));
        case lexer::keywords::NEW:
          return parse_nary_expression(filename, tokens, begin);
        default: {
          output<expression> out;
          out.filename = filename;
          out.contexts.push_back(tokens[begin].token_context);
          out.next_token = begin + 1;
          message_builder builder;
          builder.builder << "Invalid keyword "
                          << lexer::all_mappings
                                 .keywords_to_strings[static_cast<std::size_t>(
                                     tokens[begin].token_data.as_keyword)]
                          << " in start of expression!";
          out.messages.push_back(builder.build_message(
              logger::level::FATAL_ERROR, tokens[begin].token_context));
          return out;
        }
      }
    case lexer::token::data::type::OPERATOR_TOKEN:
      switch (tokens[begin].token_data.as_operator) {
        case lexer::operators::ROUND_OPEN:
          return output<expression>::generalize(
              parse<parenthetical>(filename, tokens, begin));
        case lexer::operators::ADD:
        case lexer::operators::DEC:
        case lexer::operators::INC:
        case lexer::operators::BITNOT:
        case lexer::operators::LNOT: {
          return parse_nary_expression(filename, tokens, begin);
        }
        default: {
          output<expression> out;
          out.filename = filename;
          out.contexts.push_back(tokens[begin].token_context);
          out.next_token = begin + 1;
          message_builder builder;
          builder.builder << "Invalid operator '"
                          << lexer::all_mappings
                                 .operators_to_strings[static_cast<std::size_t>(
                                     tokens[begin].token_data.as_operator)]
                          << "' in start of expression!";
          out.messages.push_back(builder.build_message(
              logger::level::FATAL_ERROR, tokens[begin].token_context));
          return out;
        }
      }
    case lexer::token::data::type::LITERAL_TOKEN:
    case lexer::token::data::type::DEFERRED_TOKEN:
      return parse_nary_expression(filename, tokens, begin);
  }
}

parse_decl(unary_expression) {
  output<unary_expression> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);

  return out;
}

output<expression> oops_compiler::parser::parse_nary_expression(
    const char *filename, const std::vector<lexer::token> &tokens,
    std::size_t begin) {
  switch (tokens[begin].token_data.token_type) {
    case lexer::token::data::type::DEFERRED_TOKEN: {
      if (begin == tokens.size() - 1) {
        return output<expression>::generalize(parse<identifier_expression>(
            filename, tokens, begin));
      }
      switch (tokens[begin + 1].token_data.token_type) {
        case lexer::token::data::type::OPERATOR_TOKEN: {
          switch (tokens[begin + 1].token_data.as_operator) {
            case lexer::operators::CURLY_CLOSE:
            case lexer::operators::ROUND_CLOSE:
            case lexer::operators::SQUARE_CLOSE:
            case lexer::operators::SEMICOLON:
            case lexer::operators::COLON:
              return output<expression>::generalize(
                  parse<identifier_expression>(filename, tokens, begin));
          }
        }
      }
    }
  }
}