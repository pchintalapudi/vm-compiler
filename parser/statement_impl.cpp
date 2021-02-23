#include <sstream>

#include "../lexer/converters.h"
#include "access_expression.h"
#include "basic_block.h"
#include "classloader.h"
#include "declaration.h"
#include "enhanced_for.h"  // TODO
#include "expression.h"
#include "for.h"  // TODO
#include "ifelse.h"
#include "impl_common.h"
#include "returns.h"
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
parse_decl(switch_statement);
parse_decl(case_statement);
parse_decl(default_statement);
parse_decl(type_instantiation);
parse_decl(declaration);
parse_decl(general_type);

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
  auto conditional = parse<parenthetical>(filename, tokens, begin, classes);
  std::copy(conditional.messages.begin(), conditional.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = conditional.next_token;
  if (!conditional.value) {
    return out;
  }
  begin = out.next_token;
  auto stmnt = parse<statement>(filename, tokens, begin, classes);
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
  auto conditional = parse<parenthetical>(filename, tokens, begin, classes);
  std::copy(conditional.messages.begin(), conditional.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = conditional.next_token;
  if (!conditional.value) {
    return out;
  }
  begin = out.next_token;
  auto stmnt = parse<statement>(filename, tokens, begin, classes);
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
  auto stmnt = parse<statement>(filename, tokens, begin, classes);
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
  if (tokens[begin].token_data.token_type ==
          lexer::token::data::type::OPERATOR_TOKEN &&
      tokens[begin].token_data.as_operator == lexer::operators::CURLY_CLOSE) {
    out.contexts.push_back(tokens[begin].token_context);
    out.next_token = begin + 1;
    out.value = std::make_unique<basic_block>(
        std::vector<std::unique_ptr<statement>>{});
    return out;
  }
  std::vector<std::unique_ptr<statement>> statements;
  do {
    auto stmnt = parse<statement>(filename, tokens, begin, classes);
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
  out.value = std::make_unique<basic_block>(std::move(statements));
  return out;
}

parse_decl(return_statement) {
  output<return_statement> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  begin++;
  auto stmnt = parse<semicolon_statement>(filename, tokens, begin, classes);
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
  auto stmnt = parse<semicolon_statement>(filename, tokens, begin, classes);
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
  auto expr = parse<expression>(filename, tokens, begin, classes);
  std::copy(expr.messages.begin(), expr.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = expr.next_token;
  if (!expr.value) {
    return out;
  }
  out.value = std::make_unique<semicolon_statement>(std::move(*expr.value));
  return out;
}

std::variant<output<for_statement>, output<enhanced_for_statement>>
parse_generalized_for(const char *filename,
                      const std::vector<oops_compiler::lexer::token> &tokens,
                      std::size_t begin,
                      std::unordered_set<std::string> &classes);

parse_decl(statement) {
  switch (tokens[begin].token_data.token_type) {
    case lexer::token::data::type::KEYWORD_TOKEN: {
      switch (tokens[begin].token_data.as_keyword) {
        case lexer::keywords::BREAK:
        case lexer::keywords::CONTINUE:
        case lexer::keywords::NEW:
          break;
        case lexer::keywords::FOR: {
          auto out = parse_generalized_for(filename, tokens, begin, classes);
          return std::holds_alternative<output<for_statement>>(out)
                     ? output<statement>::generalize(
                           std::move(std::get<output<for_statement>>(out)))
                     : output<statement>::generalize(std::move(
                           std::get<output<enhanced_for_statement>>(out)));
        }
        case lexer::keywords::WHILE:
          return output<statement>::generalize(
              parse<while_statement>(filename, tokens, begin, classes));
        case lexer::keywords::ELSE:
          return output<statement>::generalize(
              parse<else_statement>(filename, tokens, begin, classes));
        case lexer::keywords::IF:
          return output<statement>::generalize(
              parse<if_statement>(filename, tokens, begin, classes));
        case lexer::keywords::RETURN:
          return output<statement>::generalize(
              parse<return_statement>(filename, tokens, begin, classes));
        case lexer::keywords::SWITCH:
          return output<statement>::generalize(
              parse<switch_statement>(filename, tokens, begin, classes));
        case lexer::keywords::THROW:
          return output<statement>::generalize(
              parse<throw_statement>(filename, tokens, begin, classes));
        case lexer::keywords::AS:
        case lexer::keywords::CASE:
        case lexer::keywords::CLASS:
        case lexer::keywords::CONSTRUCTOR:
        case lexer::keywords::DEFAULT:
        case lexer::keywords::DEF:
        case lexer::keywords::DO:  // TODO
        case lexer::keywords::ENUM:
        case lexer::keywords::EXTENDS:
        case lexer::keywords::IS:
        case lexer::keywords::FINAL:
        case lexer::keywords::TRY:      // TODO
        case lexer::keywords::CATCH:    // TODO
        case lexer::keywords::FINALLY:  // TODO
        case lexer::keywords::GET:
        case lexer::keywords::GOTO:
        case lexer::keywords::IMPORT:
        case lexer::keywords::IMPLEMENTS:
        case lexer::keywords::INTERFACE:
        case lexer::keywords::INSTANCEOF:
        case lexer::keywords::INTRINSIC:
        case lexer::keywords::NATIVE:
        case lexer::keywords::OPERATOR:
        case lexer::keywords::PACKAGE:
        case lexer::keywords::PRIVATE:
        case lexer::keywords::PROTECTED:
        case lexer::keywords::PUBLIC:
        case lexer::keywords::SET:
        case lexer::keywords::STATIC:
        case lexer::keywords::USING:  // TODO
        case lexer::keywords::__COUNT__: {
          message_builder builder;
          builder.builder << "Keyword "
                          << lexer::all_mappings
                                 .keywords_to_strings[static_cast<std::size_t>(
                                     tokens[begin].token_data.as_keyword)]
                          << " cannot begin a statement!";
          if (begin < tokens.size()) {
            auto out = parse<statement>(filename, tokens, begin + 1, classes);
            out.messages.push_back(builder.build_message(
                logger::level::ERROR, tokens[begin].token_context));
            return out;
          } else {
            output<statement> out;
            out.filename = filename;
            out.contexts = {tokens[begin].token_context};
            out.messages.push_back(builder.build_message(
                logger::level::ERROR, tokens[begin].token_context));
            out.next_token = begin + 1;
            return out;
          }
        }
      }
      break;
    }
    case lexer::token::data::type::LITERAL_TOKEN:
      break;
    case lexer::token::data::type::OPERATOR_TOKEN: {
      switch (tokens[begin].token_data.as_operator) {
        default: {
          message_builder builder;
          builder.builder << "Operator "
                          << lexer::all_mappings
                                 .operators_to_strings[static_cast<std::size_t>(
                                     tokens[begin].token_data.as_keyword)]
                          << " cannot begin a statement!";
          if (begin < tokens.size()) {
            auto out = parse<statement>(filename, tokens, begin + 1, classes);
            out.messages.push_back(builder.build_message(
                logger::level::ERROR, tokens[begin].token_context));
            return out;
          } else {
            output<statement> out;
            out.filename = filename;
            out.contexts = {tokens[begin].token_context};
            out.messages.push_back(builder.build_message(
                logger::level::ERROR, tokens[begin].token_context));
            out.next_token = begin + 1;
            return out;
          }
        }
        case lexer::operators::CURLY_OPEN:
          return output<statement>::generalize(
              parse<basic_block>(filename, tokens, begin, classes));
        case lexer::operators::ROUND_OPEN:
        case lexer::operators::ADD:
        case lexer::operators::BITNOT:
        case lexer::operators::DEC:
        case lexer::operators::LNOT:
        case lexer::operators::SEMICOLON:
        case lexer::operators::SUB:
          break;
      }
      break;
    }
    case lexer::token::data::type::IDENTIFIER_TOKEN: {
      auto str = std::string(tokens[begin].token_data.as_identifier.start,
                             tokens[begin].token_data.as_identifier.size);
      if (classes.find(str) != classes.end()) {
        if (tokens.size() - begin == 1) {
          output<statement> out;
          out.filename = filename;
          out.contexts.push_back(tokens[begin].token_context);
          out.next_token = begin + 1;
          message_builder builder;
          builder.builder << "Unexpected end of statement!";
          out.messages.push_back(builder.build_message(
              logger::level::FATAL_ERROR, tokens[begin].token_context));
          return out;
        }
        output<general_type> maybetype =
            parse<general_type>(filename, tokens, begin, classes);
        if (!maybetype.value) {
          break;
        }
        if (maybetype.next_token == tokens.size()) {
          break;
        }
        if (tokens[maybetype.next_token].token_data.token_type !=
            lexer::token::data::type::IDENTIFIER_TOKEN) {
          break;
        }
        return output<statement>::generalize(
            parse<declaration>(filename, tokens, begin, classes));
      }
      break;
    }
  }
  return output<statement>::generalize(
      parse<semicolon_statement>(filename, tokens, begin, classes));
}

parse_decl(switch_statement) {
  output<switch_statement> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  begin++;
  if (tokens.size() == begin) {
    out.next_token = begin;
    message_builder builder;
    builder.builder << "Unexpected end of switch declaration!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  if (tokens[begin].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[begin].token_data.as_operator != lexer::operators::ROUND_OPEN) {
    out.next_token = begin;
    message_builder builder;
    builder.builder
        << "Switch declarations must have a dynamic expression in parentheses!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  output<parenthetical> on =
      parse<parenthetical>(filename, tokens, begin, classes);
  std::copy(on.messages.begin(), on.messages.end(),
            std::back_inserter(out.messages));
  begin = out.next_token = on.next_token;
  if (!on.value) {
    return out;
  }
  if (tokens[begin].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[begin].token_data.as_operator != lexer::operators::CURLY_OPEN) {
    out.next_token = begin;
    message_builder builder;
    builder.builder
        << "Switch declarations must have cases inside a braced block!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  begin++;
  std::vector<case_statement> cases;
  std::optional<default_statement> def;
  while (begin < tokens.size() && tokens[begin].token_data.token_type ==
                                      lexer::token::data::type::KEYWORD_TOKEN) {
    switch (tokens[begin].token_data.as_keyword) {
      case lexer::keywords::CASE: {
        output<case_statement> single =
            parse<case_statement>(filename, tokens, begin, classes);
        std::copy(single.messages.begin(), single.messages.end(),
                  std::back_inserter(out.messages));
        begin = out.next_token = single.next_token;
        if (!single.value) {
          return out;
        }
        cases.push_back(std::move(**single.value));
        break;
      }
      case lexer::keywords::DEFAULT: {
        output<default_statement> d =
            parse<default_statement>(filename, tokens, begin, classes);
        std::copy(d.messages.begin(), d.messages.end(),
                  std::back_inserter(out.messages));
        if (def) {
          message_builder builder;
          builder.builder
              << "Multiple default cases in the same switch statement!";
          out.messages.push_back(builder.build_message(
              logger::level::ERROR, tokens[begin].token_context));
        }
        begin = out.next_token = d.next_token;
        if (!d.value) {
          return out;
        }
        def = std::move(**d.value);
        break;
      }
      default: {
        out.next_token = begin;
        message_builder builder;
        builder.builder << "Invalid keyword in switch statement!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[begin].token_context));
        return out;
      }
    }
  }
  if (begin == tokens.size() ||
      tokens[begin].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[begin].token_data.as_operator != lexer::operators::CURLY_CLOSE) {
    out.next_token = begin;
    message_builder builder;
    builder.builder << "Unclosed switch statement!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  out.next_token = begin + 1;
  out.value =
      std::make_unique<switch_statement>(std::move(cases), std::move(def));
  return out;
}

parse_decl(default_statement) {
  output<default_statement> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  begin++;
  if (tokens.size() == begin) {
    out.next_token = begin;
    message_builder builder;
    builder.builder << "Unexpected end of default statement!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  if (tokens[begin].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[begin].token_data.as_operator != lexer::operators::COLON) {
    out.next_token = begin;
    message_builder builder;
    builder.builder << "Expected colon after default!";
    out.messages.push_back(builder.build_message(logger::level::ERROR,
                                                 tokens[begin].token_context));
    return out;
  } else {
    begin++;
  }
  output<statement> block = parse<statement>(filename, tokens, begin, classes);
  std::copy(block.messages.begin(), block.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = block.next_token;
  if (!block.value) {
    return out;
  }
  out.value = std::make_unique<default_statement>(std::move(*block.value));
  return out;
}

parse_decl(case_statement) {
  output<case_statement> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  begin++;
  if (tokens.size() == begin) {
    out.next_token = begin;
    message_builder builder;
    builder.builder << "Unexpected end of case statement!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  if (tokens[begin].token_data.token_type !=
      lexer::token::data::type::LITERAL_TOKEN) {
    out.next_token = begin;
    message_builder builder;
    builder.builder << "Case statements must contain a literal value!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  lexer::token literal = tokens[begin];
  begin++;
  if (tokens.size() == begin) {
    out.next_token = begin;
    message_builder builder;
    builder.builder << "Unexpected end of case statement!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  if (tokens[begin].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[begin].token_data.as_operator != lexer::operators::COLON) {
    out.next_token = begin;
    message_builder builder;
    builder.builder << "Expected colon after case literal!";
    out.messages.push_back(builder.build_message(logger::level::ERROR,
                                                 tokens[begin].token_context));
    return out;
  } else {
    begin++;
  }
  output<statement> block = parse<statement>(filename, tokens, begin, classes);
  std::copy(block.messages.begin(), block.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = block.next_token;
  if (!block.value) {
    return out;
  }
  out.value = std::make_unique<case_statement>(std::move(*block.value),
                                               std::move(literal));
  return out;
}

parse_decl(type_instantiation) {
  output<type_instantiation> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  std::string alias(tokens[out.next_token].token_data.as_identifier.start,
                    tokens[out.next_token].token_data.as_identifier.size);
  out.next_token++;
  if (out.next_token == tokens.size() ||
      (tokens[out.next_token].token_data.token_type !=
           lexer::token::data::type::OPERATOR_TOKEN ||
       tokens[out.next_token].token_data.as_operator !=
           lexer::operators::LESS)) {
    out.next_token++;
    bool arraytype = false;
    if (out.next_token < tokens.size() - 1 &&
        tokens[out.next_token].token_data.token_type ==
            lexer::token::data::type::OPERATOR_TOKEN &&
        tokens[out.next_token].token_data.as_operator ==
            lexer::operators::SQUARE_OPEN &&
        tokens[out.next_token + 1].token_data.token_type ==
            lexer::token::data::type::OPERATOR_TOKEN &&
        tokens[out.next_token + 1].token_data.as_operator ==
            lexer::operators::SQUARE_CLOSE) {
      arraytype = true;
    }
    out.value = std::make_unique<type_instantiation>(
        std::move(alias), std::vector<general_type>{}, arraytype);
  } else {
    std::vector<general_type> subtypes;
    do {
      out.next_token++;
      if (tokens.size() == out.next_token) {
        message_builder builder;
        builder.builder << "Unexpected end of type instantiation!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
        return out;
      }
      output<general_type> subtype =
          parse<general_type>(filename, tokens, out.next_token, classes);
      std::copy(subtype.messages.begin(), subtype.messages.end(),
                std::back_inserter(out.messages));
      out.next_token = subtype.next_token;
      if (!subtype.value) {
        return out;
      }
      subtypes.push_back(std::move(**subtype.value));
      if (tokens[out.next_token].token_data.token_type !=
              lexer::token::data::type::OPERATOR_TOKEN &&
          (tokens[out.next_token].token_data.as_operator !=
               lexer::operators::GREATER &&
           tokens[out.next_token].token_data.as_operator !=
               lexer::operators::GEQUALS &&
           tokens[out.next_token].token_data.as_operator !=
               lexer::operators::BITSRA &&
           tokens[out.next_token].token_data.as_operator !=
               lexer::operators::BITSRL &&
           tokens[out.next_token].token_data.as_operator !=
               lexer::operators::BITSRAEQ &&
           tokens[out.next_token].token_data.as_operator !=
               lexer::operators::BITSRLEQ) &&
          tokens[out.next_token].token_data.as_operator !=
              lexer::operators::COMMA) {
        message_builder builder;
        builder.builder << "Expected comma-separated type instantiations!";
        out.messages.push_back(builder.build_message(
            logger::level::ERROR, tokens[out.next_token].token_context));
      }
    } while (tokens[out.next_token].token_data.token_type !=
                 lexer::token::data::type::OPERATOR_TOKEN &&
             (tokens[out.next_token].token_data.as_operator !=
                  lexer::operators::GREATER &&
              tokens[out.next_token].token_data.as_operator !=
                  lexer::operators::GEQUALS &&
              tokens[out.next_token].token_data.as_operator !=
                  lexer::operators::BITSRA &&
              tokens[out.next_token].token_data.as_operator !=
                  lexer::operators::BITSRL &&
              tokens[out.next_token].token_data.as_operator !=
                  lexer::operators::BITSRAEQ &&
              tokens[out.next_token].token_data.as_operator !=
                  lexer::operators::BITSRLEQ));
    switch (tokens[out.next_token].token_data.as_operator) {
      case lexer::operators::GREATER:
        break;
      case lexer::operators::GEQUALS: {
        lexer::token copy = tokens[out.next_token];
        copy.token_context.char_number++;
        copy.token_context.global_char_number++;
        copy.token_data.as_operator = lexer::operators::EQUALS;
        tokens.insert(out.next_token + 1 + tokens.begin(), copy);
        break;
      }
      case lexer::operators::BITSRA: {
        lexer::token copy = tokens[out.next_token];
        copy.token_context.char_number++;
        copy.token_context.global_char_number++;
        copy.token_data.as_operator = lexer::operators::GREATER;
        tokens.insert(out.next_token + 1 + tokens.begin(), copy);
        break;
      }
      case lexer::operators::BITSRL: {
        lexer::token copy = tokens[out.next_token];
        copy.token_context.char_number++;
        copy.token_context.global_char_number++;
        copy.token_data.as_operator = lexer::operators::BITSRA;
        tokens.insert(out.next_token + 1 + tokens.begin(), copy);
        break;
      }
      case lexer::operators::BITSRAEQ: {
        lexer::token copy = tokens[out.next_token];
        copy.token_context.char_number++;
        copy.token_context.global_char_number++;
        copy.token_data.as_operator = lexer::operators::GEQUALS;
        tokens.insert(out.next_token + 1 + tokens.begin(), copy);
        break;
      }
      case lexer::operators::BITSRLEQ: {
        lexer::token copy = tokens[out.next_token];
        copy.token_context.char_number++;
        copy.token_context.global_char_number++;
        copy.token_data.as_operator = lexer::operators::BITSRAEQ;
        tokens.insert(out.next_token + 1 + tokens.begin(), copy);
        break;
      }
      default: {
        message_builder builder;
        builder.builder << "Unexpected error in type instantiation (unknown "
                           "token appeared)!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
        return out;
      }
    }
    out.next_token++;
    bool arraytype = false;
    if (out.next_token < tokens.size() - 1 &&
        tokens[out.next_token].token_data.token_type ==
            lexer::token::data::type::OPERATOR_TOKEN &&
        tokens[out.next_token].token_data.as_operator ==
            lexer::operators::SQUARE_OPEN &&
        tokens[out.next_token + 1].token_data.token_type ==
            lexer::token::data::type::OPERATOR_TOKEN &&
        tokens[out.next_token + 1].token_data.as_operator ==
            lexer::operators::SQUARE_CLOSE) {
      arraytype = true;
    }
    out.value = std::make_unique<type_instantiation>(alias, std::move(subtypes),
                                                     arraytype);
  }
  return out;
}

parse_decl(declaration) {
  output<declaration> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  output<general_type> type =
      parse<general_type>(filename, tokens, out.next_token, classes);
  std::copy(type.messages.begin(), type.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = type.next_token;
  if (!type.value) {
    return out;
  }
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Unexpected end of declaration statement!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
      lexer::token::data::type::IDENTIFIER_TOKEN) {
    message_builder builder;
    builder.builder << "Expected variable name to be an identifier!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  std::string name(tokens[out.next_token].token_data.as_identifier.start,
                   tokens[out.next_token].token_data.as_identifier.size);
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Unexpected end of declaration statement!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      (tokens[out.next_token].token_data.as_operator !=
           lexer::operators::SEMICOLON &&
       tokens[out.next_token].token_data.as_operator != lexer::operators::EQ)) {
    message_builder builder;
    builder.builder
        << "Expected semicolon or assignment after name in declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.as_operator == lexer::operators::EQ) {
    output<expression> expr =
        parse<expression>(filename, tokens, out.next_token + 1, classes);
    std::copy(expr.messages.begin(), expr.messages.end(),
              std::back_inserter(out.messages));
    out.next_token = expr.next_token;
    if (!expr.value) {
      return out;
    }
    out.value = std::make_unique<declaration>(
        std::move(**type.value), std::move(name),
        std::optional<std::unique_ptr<expression>>{std::move(*expr.value)});
  } else {
    out.value = std::make_unique<declaration>(
        std::move(**type.value), std::move(name),
        std::optional<std::unique_ptr<expression>>{});
  }
  return out;
}

parse_decl(general_type) {
  return output<general_type>::reconstruct(
      parse<access_expression>(filename, tokens, begin, classes));
}